#include "operations.h"
#include "Base/iterator.h"

static inline FlashFileSystemSDeviceIterator * GetNextSectorIterator(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                     FlashFileSystemSDeviceIterator *iterator)
{
   size_t nextSectorIndex = (iterator - &handle->Dynamic.Iterators[0] + 1) %
            __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT;

   return &handle->Dynamic.Iterators[nextSectorIndex];
}

static FlashFileSystemSDeviceState SkipBackToNotEmptyBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                           FlashFileSystemSDeviceIterator *iterator)
{
   FileSystemBlock block;
   FlashFileSystemSDeviceState operationState;

   while((operationState = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
   {
      if(IsBlockEmpty(&block) != true)
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;

      DecrementReadCursor(iterator);
   }

   return operationState;
}

static FlashFileSystemSDeviceState SkipBackToBlockOfType(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                         FlashFileSystemSDeviceIterator *iterator,
                                                         BlockDescriptor type)
{
   FileSystemBlock block;
   FlashFileSystemSDeviceState operationState;

   while((operationState = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
   {
      if(IsBlockOfType(&block, type) == true)
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;

      DecrementReadCursor(iterator);
   }

   return operationState;
}

FlashFileSystemSDeviceState SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                 FlashFileSystemSDeviceIterator *iterator,
                                                 BlockHeaderState state)
{
   FileSystemBlock block =
   {
      .AsBlock = (struct CommonBlock)
      {
         .Descriptor = BLOCK_DESCRIPTOR_HEADER,
         .AsHeader = (struct HeaderBlock)
         {
            .State = state,
            .Padding = __BYTE_EMPTY_VALUE
         }
      }
   };
   block.AsBlock.AsHeader.Crc = ComputeHeaderBlockCrc(&block);

   return WriteForwardToCurrentBlock(handle, iterator, &block);
}

FlashFileSystemSDeviceState GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                 FlashFileSystemSDeviceIterator *iterator,
                                                 BlockHeaderState *state)
{
   FlashFileSystemSDeviceState operationState;

   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* try get sector's header state */
   while((operationState = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_HEADER)) ==
            FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
   {
      FileSystemBlock block;

      __RETURN_ERROR_IF_ANY(ReadBackwardFromCurrentBlock(handle, iterator, &block));

      /* valid header found */
      if(block.AsBlock.AsHeader.Crc == ComputeHeaderBlockCrc(&block) &&
               __IS_VALID_BLOCK_HEADER_STATE(block.AsBlock.AsHeader.State) == true)
      {
         *state = block.AsBlock.AsHeader.State;
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
      }
   }

   return operationState;
}

FlashFileSystemSDeviceState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                FlashFileSystemSDeviceIterator *iterator,
                                                BlockHeaderState headerState)
{
   __RETURN_ERROR_IF_ANY(EraseSector(handle, iterator->Sector));
   SeekWriteCursor(iterator, IteratorStart(iterator));
   return SetSectorHeaderState(handle, iterator, headerState);
}

FlashFileSystemSDeviceState GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                  FlashFileSystemSDeviceIterator *iterator,
                                                  SectorInitialState *state)
{
   FlashFileSystemSDeviceState operationState;

   /* find first written block starting from end of sector */
   SeekReadCursor(iterator, IteratorEnd(iterator));
   operationState = SkipBackToNotEmptyBlock(handle, iterator);

   /* no written blocks -> sector empty, set write cursor to start of the sector */
   if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
   {
      state->IsEmpty = true;
      SeekWriteCursor(iterator, IteratorStart(iterator));
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
   }

   __RETURN_ERROR_IF_ANY(operationState);
   state->IsEmpty = false;

   /* set write cursor to next block after found written one, it'll be free or out of bounds in case of full sector */
   SeekWriteCursor(iterator, NextBlockAddress(iterator->ReadCursor));

   operationState = GetSectorHeaderState(handle, iterator, &state->HeaderState);

   if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
   {
      state->HasValidHeaderState = false;
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
   }

   __RETURN_ERROR_IF_ANY(operationState);
   state->HasValidHeaderState = true;

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState MoveVariableDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                    FlashFileSystemSDeviceAddress variableAddress)
{
   /* variable with requested address is already in cache */
   if(handle->Dynamic.VariableDataCache.Address == variableAddress)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;

   FlashFileSystemSDeviceState operationState;
   FlashFileSystemSDeviceIterator *iterator = handle->Dynamic.ActiveIterator;

   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* iterate over preamble blocks to find valid one with requested address */
   while((operationState = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_DATA_PREAMBLE)) ==
            FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
   {
      /* create temporary cache instance for storage */
      __typeof__(handle->Dynamic.VariableDataCache) cache;
      FileSystemBlock block;

      cache.MemoryAddress = iterator->ReadCursor;

      /* read preamble block */
      __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, iterator, &block));

      cache.Address = block.AsBlock.AsDataPreamble.Address;

      if(cache.Address != variableAddress)
         continue;

      cache.IsDeleted = block.AsBlock.AsDataPreamble.IsDeleted;
      cache.Size = block.AsBlock.AsDataPreamble.VariableSize;

      CrcType readCrc = block.AsBlock.AsDataPreamble.Crc,
              computedCrc = ComputeHeaderBlockCrc(&block);

      size_t leftToReadSize = cache.Size;

      if(cache.IsDeleted != true && cache.Size != 0)
      {
         /* compute crc and in-memory variable size to validate it */
         while((operationState = ReadForwardFromCurrentBlock(handle, iterator, &block)) ==
                  FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
         {
            if(IsBlockOfType(&block, BLOCK_DESCRIPTOR_VARIABLE_DATA) != true)
               break;

            size_t blockDataSize = __MIN(leftToReadSize, sizeof(block.AsBlock.AsData.Data));
            computedCrc = UpdateCrcWithDataBlock(&block, computedCrc, blockDataSize);
            leftToReadSize -= blockDataSize;

            if(leftToReadSize == 0)
               break;
         }

         if(operationState != FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
            __RETURN_ERROR_IF_ANY(operationState);
      }

      /* read size and crc are equal to computed ones, entry is valid */
      if(computedCrc == readCrc && leftToReadSize == 0)
      {
         handle->Dynamic.VariableDataCache = cache;
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
      }

      /* move read cursor to previous block of current variable's preamble */
      SeekReadCursor(iterator, PreviousBlockAddress(cache.MemoryAddress));
   }

   return operationState;
}

FlashFileSystemSDeviceState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{

   FlashFileSystemSDeviceIterator *source = handle->Dynamic.ActiveIterator;
   FlashFileSystemSDeviceIterator *target = GetNextSectorIterator(handle, source);

   /* run through all used addresses and copy variables to target sector */
   for(FlashFileSystemSDeviceAddress address = 0; address <= handle->Constant.MaxUsedAddress; address++)
   {
      FlashFileSystemSDeviceState operationState = MoveVariableDataToCache(handle, address);

      /* variable not found, maybe deleted before last transfer -> just skip it */
      if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
         continue;

      __RETURN_ERROR_IF_ANY(operationState);

      /* variable is deleted, no need to transfer it */
      if(handle->Dynamic.VariableDataCache.IsDeleted == true)
         continue;

      size_t variableBlocksCount = VariableBlocksCount(handle->Dynamic.VariableDataCache.Size);
      SeekReadCursor(source, handle->Dynamic.VariableDataCache.MemoryAddress);

      /* copy each variable's block from source to target sector */
      while(variableBlocksCount-- > 0)
      {
         FileSystemBlock block;
         __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, source, &block));
         __RETURN_ERROR_IF_ANY(WriteForwardToCurrentBlock(handle, target, &block));
      }
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}
