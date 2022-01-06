#include "operations.h"
#include "Base/iterator.h"

static inline FlashFileSystemIterator * GetNextSectorIterator(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                              FlashFileSystemIterator *iterator)
{
   size_t nextSectorIndex = (iterator - &handle->Dynamic.Iterators[0] + 1) % __FLASH_FILE_SYSTEM_SECTORS_COUNT;
   return &handle->Dynamic.Iterators[nextSectorIndex];
}

static FlashFileSystemState SkipBackToNotEmptyBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                    FlashFileSystemIterator *iterator)
{
   FileSystemBlock block;
   FlashFileSystemState state;
   while((state = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATE_OK)
   {
      if(IsBlockEmpty(&block) != true)
         return FLASH_FILE_SYSTEM_STATE_OK;

      DecrementReadCursor(iterator);
   }

   return state;
}

static FlashFileSystemState SkipBackToBlockOfType(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                  FlashFileSystemIterator *iterator,
                                                  BlockDescriptor type)
{
   FileSystemBlock block;
   FlashFileSystemState state;
   while((state = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATE_OK)
   {
      if(IsBlockOfType(&block, type) == true)
         return FLASH_FILE_SYSTEM_STATE_OK;

      DecrementReadCursor(iterator);
   }

   return state;
}

void InvalidateVariableDataCache(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   handle->Dynamic.VariableDataCache.Address = __FLASH_FILE_SYSTEM_MAX_ADDRESS + 1;
}

FlashFileSystemState SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemIterator *iterator,
                                          BlockHeaderState headerState)
{
   FileSystemBlock block =
   {
      .AsBlock = (struct CommonBlock)
      {
         .Descriptor = BLOCK_DESCRIPTOR_HEADER,
         .AsHeader = (struct HeaderBlock)
         {
            .State = headerState,
            .Padding = __BYTE_EMPTY_VALUE
         }
      }
   };
   block.AsBlock.AsHeader.Crc = ComputeHeaderBlockCrc(&block);

   return WriteForwardToCurrentBlock(handle, iterator, &block);
}

FlashFileSystemState GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemIterator *iterator,
                                          BlockHeaderState *headerState)
{
   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* try get sector's header state */
   FlashFileSystemState state;
   while((state = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_HEADER)) == FLASH_FILE_SYSTEM_STATE_OK)
   {
      FileSystemBlock block;

      __RETURN_ERROR_IF_ANY(ReadBackwardFromCurrentBlock(handle, iterator, &block));

      /* valid header found */
      if(block.AsBlock.AsHeader.Crc == ComputeHeaderBlockCrc(&block) &&
         __IS_VALID_BLOCK_HEADER_STATE(block.AsBlock.AsHeader.State) == true)
      {
         *headerState = block.AsBlock.AsHeader.State;
         return FLASH_FILE_SYSTEM_STATE_OK;
      }
   }

   return state;
}

FlashFileSystemState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                         FlashFileSystemIterator *iterator,
                                         BlockHeaderState headerState)
{
   __RETURN_ERROR_IF_ANY(EraseSector(handle, iterator->Sector));
   SeekWriteCursor(iterator, IteratorStart(iterator));
   return SetSectorHeaderState(handle, iterator, headerState);
}

FlashFileSystemState GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                           FlashFileSystemIterator *iterator,
                                           SectorInitialState *sectorState)
{
   /* find first written block starting from end of sector */
   SeekReadCursor(iterator, IteratorEnd(iterator));
   FlashFileSystemState state;
   state = SkipBackToNotEmptyBlock(handle, iterator);

   /* no written blocks -> sector empty, set write cursor to start of the sector */
   if(state == FLASH_FILE_SYSTEM_STATE_VALUE_NOT_FOUND_ERROR)
   {
      sectorState->IsEmpty = true;
      SeekWriteCursor(iterator, IteratorStart(iterator));
      return FLASH_FILE_SYSTEM_STATE_OK;
   }

   __RETURN_ERROR_IF_ANY(state);
   sectorState->IsEmpty = false;

   /* set write cursor to next block after found written one, it'll be free or out of bounds in case of full sector */
   SeekWriteCursor(iterator, NextBlockAddress(iterator->ReadCursor));

   state = GetSectorHeaderState(handle, iterator, &sectorState->HeaderState);

   if(state == FLASH_FILE_SYSTEM_STATE_VALUE_NOT_FOUND_ERROR)
   {
      sectorState->HasValidHeaderState = false;
      return FLASH_FILE_SYSTEM_STATE_OK;
   }

   __RETURN_ERROR_IF_ANY(state);
   sectorState->HasValidHeaderState = true;

   return FLASH_FILE_SYSTEM_STATE_OK;
}

FlashFileSystemState MoveVariableDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                             FlashFileSystemAddress variableAddress)
{
   /* variable with requested address is already in cache */
   if(handle->Dynamic.VariableDataCache.Address == variableAddress)
      return FLASH_FILE_SYSTEM_STATE_OK;

   FlashFileSystemState state;
   FlashFileSystemIterator *iterator = handle->Dynamic.ActiveIterator;

   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* iterate over preamble blocks to find valid one with requested address */
   while((state = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_PREAMBLE)) == FLASH_FILE_SYSTEM_STATE_OK)
   {
      /* create temporary cache instance for storage */
      __typeof__(handle->Dynamic.VariableDataCache) cache;
      FileSystemBlock block;

      cache.MemoryAddress = iterator->ReadCursor;

      /* read preamble block */
      __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, iterator, &block));

      cache.Address = block.AsBlock.AsDataPreamble.Address;

      if(cache.Address != variableAddress)
      {
         SeekReadCursor(iterator, PreviousBlockAddress(cache.MemoryAddress));
         continue;
      }

      cache.IsDeleted = block.AsBlock.AsDataPreamble.IsDeleted;
      cache.Size = block.AsBlock.AsDataPreamble.VariableSize;

      CrcType readCrc = block.AsBlock.AsDataPreamble.Crc,
              computedCrc = ComputeHeaderBlockCrc(&block);

      size_t leftToReadSize = cache.Size;

      if(cache.IsDeleted != true && cache.Size != 0)
      {
         /* compute crc and in-memory variable size to validate it */
         while((state = ReadForwardFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATE_OK)
         {
            if(IsBlockOfType(&block, BLOCK_DESCRIPTOR_VARIABLE_DATA) != true)
               break;

            size_t blockDataSize = __MIN(leftToReadSize, sizeof(block.AsBlock.AsData.Data));
            computedCrc = UpdateCrcWithDataBlock(&block, computedCrc, blockDataSize);
            leftToReadSize -= blockDataSize;

            if(leftToReadSize == 0)
               break;
         }

         if(state != FLASH_FILE_SYSTEM_STATE_VALUE_NOT_FOUND_ERROR)
            __RETURN_ERROR_IF_ANY(state);
      }

      /* read size and crc are equal to computed ones, entry is valid */
      if(computedCrc == readCrc && leftToReadSize == 0)
      {
         handle->Dynamic.VariableDataCache = cache;
         return FLASH_FILE_SYSTEM_STATE_OK;
      }

      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_CORRUPTED_STATE);

      /* move read cursor to previous block of current variable's preamble */
      SeekReadCursor(iterator, PreviousBlockAddress(cache.MemoryAddress));
   }

   return state;
}

FlashFileSystemState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   FlashFileSystemIterator *source = handle->Dynamic.ActiveIterator;
   FlashFileSystemIterator *target = GetNextSectorIterator(handle, source);

   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_TRANSFER_ONGOING));

   /* run through all used addresses and copy variables to target sector */
   for(FlashFileSystemAddress address = 0; address <= handle->Constant->MaxUsedAddress; address++)
   {
      FlashFileSystemState state = MoveVariableDataToCache(handle, address);

      /* variable not found, maybe deleted before last transfer -> just skip it */
      if(state == FLASH_FILE_SYSTEM_STATE_VALUE_NOT_FOUND_ERROR)
         continue;

      __RETURN_ERROR_IF_ANY(state);

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

   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_TRANSFER_END));
   __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, source, HEADER_STATE_ERASED));
   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_ACTIVE));
   InvalidateVariableDataCache(handle);
   handle->Dynamic.ActiveIterator = target;

   return FLASH_FILE_SYSTEM_STATE_OK;
}
