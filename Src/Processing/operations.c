#include "operations.h"
#include "Base/iterator.h"

static inline size_t GetNextSectorIteratorIndex(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                FlashFileSystemIterator *iterator)
{
   size_t nextIteratorIndex = (iterator - &handle->Runtime.Iterators[0] + 1) % __FLASH_FILE_SYSTEM_SECTORS_COUNT;
   return nextIteratorIndex;
}

static FlashFileSystemStatus SkipBackToNotEmptyBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                     FlashFileSystemIterator *iterator)
{
   FileSystemBlock block;
   FlashFileSystemStatus state;
   while((state = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATUS_OK)
   {
      if(IsBlockEmpty(handle, &block) != true)
         return FLASH_FILE_SYSTEM_STATUS_OK;

      DecrementReadCursor(iterator);
   }

   return state;
}

static FlashFileSystemStatus SkipBackToBlockOfType(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                   FlashFileSystemIterator *iterator,
                                                   BlockDescriptor type)
{
   FileSystemBlock block;
   FlashFileSystemStatus state;
   while((state = PeekFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATUS_OK)
   {
      if(IsBlockOfType(&block, type) == true)
         return FLASH_FILE_SYSTEM_STATUS_OK;

      DecrementReadCursor(iterator);
   }

   return state;
}

void InvalidateFileDataCache(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   handle->Runtime.FileDataCache.Address = __FLASH_FILE_SYSTEM_MAX_ADDRESS + 1;
}

FlashFileSystemStatus SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
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
            .Padding = ErasedByteValue(handle)
         }
      }
   };
   block.AsBlock.AsHeader.Crc = ComputeHeaderBlockCrc(&block);

   return WriteForwardToCurrentBlock(handle, iterator, &block);
}

FlashFileSystemStatus GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                           FlashFileSystemIterator *iterator,
                                           BlockHeaderState *headerState)
{
   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* try get sector's header state */
   FlashFileSystemStatus state;
   while((state = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_HEADER)) == FLASH_FILE_SYSTEM_STATUS_OK)
   {
      FileSystemBlock block;

      __RETURN_ERROR_IF_ANY(ReadBackwardFromCurrentBlock(handle, iterator, &block));

      /* valid header found */
      if(block.AsBlock.AsHeader.Crc == ComputeHeaderBlockCrc(&block) &&
         __IS_VALID_BLOCK_HEADER_STATE(block.AsBlock.AsHeader.State) == true)
      {
         *headerState = block.AsBlock.AsHeader.State;
         return FLASH_FILE_SYSTEM_STATUS_OK;
      }
   }

   return state;
}

FlashFileSystemStatus FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemIterator *iterator,
                                          BlockHeaderState headerState)
{
   __RETURN_ERROR_IF_ANY(EraseSector(handle, &__ITERATOR_SECTOR(handle, iterator)));
   SeekWriteCursor(iterator, IteratorStart(handle, iterator));
   return SetSectorHeaderState(handle, iterator, headerState);
}

FlashFileSystemStatus GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                           FlashFileSystemIterator *iterator,
                                           SectorInitialState *sectorState)
{
   /* find first written block starting from end of sector */
   SeekReadCursor(iterator, IteratorEnd(handle, iterator));
   FlashFileSystemStatus state;
   state = SkipBackToNotEmptyBlock(handle, iterator);

   /* no written blocks -> sector empty, set write cursor to start of the sector */
   if(state == FLASH_FILE_SYSTEM_STATUS_FILE_NOT_FOUND_ERROR)
   {
      sectorState->IsEmpty = true;
      SeekWriteCursor(iterator, IteratorStart(handle, iterator));
      return FLASH_FILE_SYSTEM_STATUS_OK;
   }

   __RETURN_ERROR_IF_ANY(state);
   sectorState->IsEmpty = false;

   /* set write cursor to next block after found written one, it'll be free or out of bounds in case of full sector */
   SeekWriteCursor(iterator, NextBlockAddress(iterator->ReadCursor));

   state = GetSectorHeaderState(handle, iterator, &sectorState->HeaderState);

   if(state == FLASH_FILE_SYSTEM_STATUS_FILE_NOT_FOUND_ERROR)
   {
      sectorState->HasValidHeaderState = false;
      return FLASH_FILE_SYSTEM_STATUS_OK;
   }

   __RETURN_ERROR_IF_ANY(state);
   sectorState->HasValidHeaderState = true;

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

FlashFileSystemStatus MoveFileDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemAddress fileAddress)
{
   /* file with requested address is already in cache */
   if(handle->Runtime.FileDataCache.Address == fileAddress)
      return FLASH_FILE_SYSTEM_STATUS_OK;

   FlashFileSystemStatus state;
   FlashFileSystemIterator *iterator = &__ACTIVE_ITERATOR(handle);

   /* write cursor is always on first free block of the sector */
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));

   /* iterate over preamble blocks to find valid one with requested address */
   while((state = SkipBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_PREAMBLE)) == FLASH_FILE_SYSTEM_STATUS_OK)
   {
      /* create temporary cache instance for storage */
      __typeof__(handle->Runtime.FileDataCache) cache;
      FileSystemBlock block;

      cache.MemoryAddress = iterator->ReadCursor;

      /* read preamble block */
      __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, iterator, &block));

      cache.Address = block.AsBlock.AsDataPreamble.Address;

      if(cache.Address != fileAddress)
      {
         SeekReadCursor(iterator, PreviousBlockAddress(cache.MemoryAddress));
         continue;
      }

      cache.IsDeleted = block.AsBlock.AsDataPreamble.IsDeleted;
      cache.Size = block.AsBlock.AsDataPreamble.FileSize;

      CrcType readCrc = block.AsBlock.AsDataPreamble.Crc,
              computedCrc = ComputeHeaderBlockCrc(&block);

      size_t leftToReadSize = cache.Size;

      if(cache.IsDeleted != true && cache.Size != 0)
      {
         /* compute crc and in-memory file size to validate it */
         while((state = ReadForwardFromCurrentBlock(handle, iterator, &block)) == FLASH_FILE_SYSTEM_STATUS_OK)
         {
            if(IsBlockOfType(&block, BLOCK_DESCRIPTOR_VARIABLE_DATA) != true)
               break;

            size_t blockDataSize = __MIN(leftToReadSize, sizeof(block.AsBlock.AsData.Data));
            computedCrc = UpdateCrcWithDataBlock(&block, computedCrc, blockDataSize);
            leftToReadSize -= blockDataSize;

            if(leftToReadSize == 0)
               break;
         }

         if(state != FLASH_FILE_SYSTEM_STATUS_FILE_NOT_FOUND_ERROR)
            __RETURN_ERROR_IF_ANY(state);
      }

      /* read size and crc are equal to computed ones, entry is valid */
      if(computedCrc == readCrc && leftToReadSize == 0)
      {
         handle->Runtime.FileDataCache = cache;
         return FLASH_FILE_SYSTEM_STATUS_OK;
      }

      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_CORRUPTED_STATE);

      /* move read cursor to previous block of current file's preamble */
      SeekReadCursor(iterator, PreviousBlockAddress(cache.MemoryAddress));
   }

   return state;
}

FlashFileSystemStatus TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   FlashFileSystemIterator *source = &__ACTIVE_ITERATOR(handle);
   size_t nextIteratorIndex = GetNextSectorIteratorIndex(handle, source);
   FlashFileSystemIterator *target = &handle->Runtime.Iterators[nextIteratorIndex];

   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_TRANSFER_ONGOING));

   /* run through all used addresses and copy file to target sector */
   for(FlashFileSystemAddress address = 0; address <= handle->Init.MaxUsedAddress; address++)
   {
      FlashFileSystemStatus state = MoveFileDataToCache(handle, address);

      /* file not found, maybe deleted before last transfer -> just skip it */
      if(state == FLASH_FILE_SYSTEM_STATUS_FILE_NOT_FOUND_ERROR)
         continue;

      __RETURN_ERROR_IF_ANY(state);

      /* file is deleted, no need to transfer it */
      if(handle->Runtime.FileDataCache.IsDeleted == true)
         continue;

      size_t fileBlocksCount = FileBlocksCount(handle->Runtime.FileDataCache.Size);
      SeekReadCursor(source, handle->Runtime.FileDataCache.MemoryAddress);

      /* copy each file's block from source to target sector */
      while(fileBlocksCount-- > 0)
      {
         FileSystemBlock block;
         __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, source, &block));
         __RETURN_ERROR_IF_ANY(WriteForwardToCurrentBlock(handle, target, &block));
      }
   }

   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_TRANSFER_END));
   __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, source, HEADER_STATE_ERASED));
   __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, target, HEADER_STATE_ACTIVE));
   InvalidateFileDataCache(handle);
   handle->Runtime.ActiveIteratorIndex = nextIteratorIndex;

   return FLASH_FILE_SYSTEM_STATUS_OK;
}
