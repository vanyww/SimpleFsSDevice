#include "helpers.h"
#include "../Base/iterator.h"

void SimpleFsSDeviceSetSectorHeaderState(__SDEVICE_HANDLE(SimpleFs) *handle,
                                            Iterator *iterator,
                                            SectorHeaderState headerState)
{
   Block headerBlock =
   {
      .AsBlock =
      {
         .Descriptor = BLOCK_DESCRIPTOR_HEADER,
         .AsHeader =
         {
            .State = headerState,
            .Padding = GetErasedByteValue(handle)
         }
      }
   };
   headerBlock.AsBlock.AsHeader.Crc = ComputeServiceBlockCrc(&headerBlock);

   if(IteratorTryWriteForward(handle, iterator, &headerBlock) != true)
      SDeviceThrow(handle, SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY);
}

bool SimpleFsSDeviceTryGetSectorHeaderState(__SDEVICE_HANDLE(SimpleFs) *handle,
                                            Iterator *iterator,
                                            SectorHeaderState *headerState)
{
   /* write cursor is always on first free block of the sector */
   SeekIteratorReadCursor(iterator, GetPreviousBlockAddress(iterator->WriteCursor));

   /* iterate over preamble blocks to find header */
   for(;;)
   {
      if(IteratorTryReadBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_HEADER) != true)
         return false;

      Block headerBlock;
      IteratorTryReadBackward(handle, iterator, &headerBlock);

      /* if valid header found */
      if(headerBlock.AsBlock.AsHeader.Crc == ComputeServiceBlockCrc(&headerBlock) &&
         __IS_VALID_SECTOR_HEADER_STATE(headerBlock.AsBlock.AsHeader.State) == true)
      {
         *headerState = headerBlock.AsBlock.AsHeader.State;
         return true;
      }

      SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_HEADER);
   }
}

void SimpleFsSDeviceFormatSectorToState(__SDEVICE_HANDLE(SimpleFs) *handle,
                                        Iterator *iterator,
                                        SectorHeaderState headerState)
{
   EraseSector(handle, iterator->Sector);
   SeekIteratorWriteCursor(iterator, GetIteratorStartAddress(iterator));
   SimpleFsSDeviceSetSectorHeaderState(handle, iterator, headerState);
}

bool SimpleFsSDeviceTryMoveFileDataToCache(__SDEVICE_HANDLE(SimpleFs) *handle, SimpleFsSDeviceAddress fileAddress)
{
   /* check if file with requested address is already in cache */
   if(handle->Runtime.FileDataCache.IsValid == true && handle->Runtime.FileDataCache.Address == fileAddress)
      return true;

   Iterator *iterator = handle->Runtime.ActiveIterator;

   /* write cursor is always on first free block of the sector */
   SeekIteratorReadCursor(iterator, GetPreviousBlockAddress(iterator->WriteCursor));

   /* iterate over preamble blocks to find valid one with requested address */
   for(;;)
   {
      if(IteratorTryReadBackToBlockOfType(handle, iterator, BLOCK_DESCRIPTOR_PREAMBLE) != true)
         return false;

      Block block;
      IteratorTryPeek(handle, iterator, &block);
      FileData fileData =
      {
         .MemoryAddress = iterator->ReadCursor,
         .Address = block.AsBlock.AsDataPreamble.Address,
         .Crc = block.AsBlock.AsDataPreamble.Crc,
         .Size = block.AsBlock.AsDataPreamble.FileSize,
         .IsDeleted = block.AsBlock.AsDataPreamble.IsDeleted
      };

      if(fileData.Address != fileAddress)
      {
         DecrementIteratorReadCursor(iterator);
         continue;
      }

      IncrementIteratorReadCursor(iterator);
      CrcType computedCrc = ComputeServiceBlockCrc(&block);
      size_t fileDataSizeToRead = fileData.Size;

      /* compute full file crc to validate it */
      while(IteratorTryReadForward(handle, iterator, &block) == true &&
            IsBlockOfType(&block, BLOCK_DESCRIPTOR_VARIABLE_DATA) == true)
      {
         size_t blockDataSize = __MIN(fileDataSizeToRead, sizeof(block.AsBlock.AsData.Data));
         computedCrc = UpdateCrcWithDataBlock(&block, computedCrc, blockDataSize);
         fileDataSizeToRead -= blockDataSize;

         if(fileDataSizeToRead == 0)
         {
            /* crc match, file is valid */
            if(computedCrc == fileData.Crc)
            {
               fileData.IsValid = true;
               handle->Runtime.FileDataCache = fileData;
               return true;
            }

            break;
         }
      }

      SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_FILE);

      /* move read cursor to previous block of current file's preamble */
      SeekIteratorReadCursor(iterator, GetPreviousBlockAddress(fileData.MemoryAddress));
   }
}

void SimpleFsSDeviceTransferSectors(__SDEVICE_HANDLE(SimpleFs) *handle)
{
   Iterator *sourceIterator = handle->Runtime.ActiveIterator;
   Iterator *targetIterator = GetNextSectorIterator(handle, sourceIterator);

   SimpleFsSDeviceSetSectorHeaderState(handle, targetIterator, SECTOR_HEADER_STATE_TRANSFER_ONGOING);

   /* run through all used addresses and copy file to target sector */
   for(SimpleFsSDeviceAddress address = 0; address <= handle->Init.MaxUsedAddress; address++)
   {
      /* if file wasn't found or is marked as deleted, skip this address */
      if(SimpleFsSDeviceTryMoveFileDataToCache(handle, address) != true ||
         handle->Runtime.FileDataCache.IsDeleted == true)
         continue;

      size_t fileBlocksCount = GetFileBlocksCount(handle->Runtime.FileDataCache.Size);
      SeekIteratorReadCursor(sourceIterator, handle->Runtime.FileDataCache.MemoryAddress);

      /* copy each file's block from source to target sector */
      while(fileBlocksCount-- > 0)
      {
         Block fileBlock;
         IteratorTryReadForward(handle, sourceIterator, &fileBlock);
         if(IteratorTryWriteForward(handle, targetIterator, &fileBlock) != true)
            SDeviceThrow(handle, SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY);
      }
   }

   SimpleFsSDeviceSetSectorHeaderState(handle, targetIterator, SECTOR_HEADER_STATE_TRANSFER_END);
   SimpleFsSDeviceFormatSectorToState(handle, sourceIterator, SECTOR_HEADER_STATE_ERASED);
   SimpleFsSDeviceSetSectorHeaderState(handle, targetIterator, SECTOR_HEADER_STATE_ACTIVE);

   handle->Runtime.FileDataCache.IsValid = false;
   handle->Runtime.ActiveIterator = targetIterator;
}

void SimpleFsSDeviceClearMemoryState(__SDEVICE_HANDLE(SimpleFs) *handle)
{
   for(size_t i = 0; i < __SIMPLE_FS_SDEVICE_SECTORS_COUNT; i++)
      SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[i], SECTOR_HEADER_STATE_ERASED);

   SimpleFsSDeviceSetSectorHeaderState(handle, &handle->Runtime.Iterators[0], SECTOR_HEADER_STATE_ACTIVE);

   handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[0];
}
