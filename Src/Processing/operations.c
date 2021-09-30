#include "operations.h"

FlashFileSystemSDeviceState ReadBlockToBuffer(__SDEVICE_HANDLE(FlashFileSystem) *handle, intptr_t address)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   if(handle->Constant->TryReadFromFlash(handle, address, BlocksSize(1), dataBlock) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState WriteBlockFromBuffer(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                 FlashFileSystemSDeviceSectorDynamicData *sector)
{
   if(sector->FirstFreeBlockAddress > SectorLastBlockAddress(sector->Sector))
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OUT_OF_MEMORY_ERROR;

   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   if(handle->Constant->TryWriteToFlash(handle, sector->FirstFreeBlockAddress, BlocksSize(1), dataBlock) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;

   sector->FirstFreeBlockAddress = NextBlockAddress(sector->FirstFreeBlockAddress);

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

static FlashFileSystemSDeviceState GetFirstEmptyBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                      const FlashFileSystemSDeviceSector *sector,
                                                      intptr_t *blockAddress)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   intptr_t searchAddress = SectorLastBlockAddress(sector);

   for(; searchAddress >= sector->StartAddress; searchAddress = PreviousBlockAddress(searchAddress))
   {
      __RETURN_ERROR_IF_ANY(ReadBlockToBuffer(handle, searchAddress));

      if(IsBlockEmpty(dataBlock) != true)
      {
         if(searchAddress == SectorLastBlockAddress(sector))
            return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

         *blockAddress = NextBlockAddress(searchAddress);
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
      }
   }

   *blockAddress = sector->StartAddress;
   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

static FlashFileSystemSDeviceState GetPreviousBlockOfType(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                          const FlashFileSystemSDeviceSector *sector,
                                                          BlockDescriptor type,
                                                          intptr_t *blockAddress)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   intptr_t searchAddress = PreviousBlockAddress(*blockAddress);

   for(; searchAddress >= sector->StartAddress; searchAddress = PreviousBlockAddress(searchAddress))
   {
      __RETURN_ERROR_IF_ANY(ReadBlockToBuffer(handle, searchAddress));

      if(IsBlockOfType(dataBlock, type) == true)
      {
         *blockAddress = searchAddress;
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
      }
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;
}

FlashFileSystemSDeviceState SetSectorState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                           FlashFileSystemSDeviceSectorDynamicData *sector,
                                           BlockHeaderState headerState)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;

   dataBlock->AsBlock = (struct CommonBlock)
   {
      .Descriptor = BLOCK_DESCRIPTOR_HEADER,
      .AsHeader = (struct HeaderBlock)
      {
         .State = headerState,
         .Padding = __BYTE_EMPTY_VALUE
      }
   };
   dataBlock->AsBlock.AsHeader.Crc = ComputeHeaderBlockCrc(dataBlock);

   return WriteBlockFromBuffer(handle, sector);
}

FlashFileSystemSDeviceState GetSectorState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                           const FlashFileSystemSDeviceSector *sector,
                                           SectorInitialState *state)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   FlashFileSystemSDeviceState operationState;

   operationState = GetFirstEmptyBlock(handle, sector, &state->FirstEmptyBlockAddress);

   if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
   {
      state->IsEmpty = true;
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
   }

   __RETURN_ERROR_IF_ANY(operationState);
   intptr_t address = state->FirstEmptyBlockAddress;

   do
   {
      operationState = GetPreviousBlockOfType(handle, sector, BLOCK_DESCRIPTOR_HEADER, &address);

      if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
      {
         state->HasValidState = false;
         state->IsEmpty = false;
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
      }

      __RETURN_ERROR_IF_ANY(operationState);
   } while(dataBlock->AsBlock.AsHeader.Crc != ComputeHeaderBlockCrc(dataBlock));

   state->HasValidState = true;
   state->HeaderState = dataBlock->AsBlock.AsHeader.State;

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                FlashFileSystemSDeviceSectorDynamicData *sector,
                                                BlockHeaderState headerState)
{
   if(handle->Constant->TryEraseFlashSector(handle, sector->Sector) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;

   sector->FirstFreeBlockAddress = sector->Sector->StartAddress;
   __RETURN_ERROR_IF_ANY(SetSectorState(handle, sector, headerState));

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState CachePreambleBlockDataWithAddress(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                              FlashFileSystemSDeviceAddress variableAddress)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;
   intptr_t preambleAddress, searchAddress = handle->Dynamic.ActiveSector->FirstFreeBlockAddress;
   CrcType computedCrc, readCrc;
   size_t readBlocksCount, computedBlocksCount, variableSize;
   bool isDeleted;

   if(handle->Dynamic.VariableDataCache.VariableAddress == variableAddress)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;

   do
   {
      __RETURN_ERROR_IF_ANY(GetPreviousBlockOfType(handle,
                                                   handle->Dynamic.ActiveSector->Sector,
                                                   BLOCK_DESCRIPTOR_DATA_PREAMBLE,
                                                   &searchAddress));

      if(dataBlock->AsBlock.AsDataPreamble.Address != variableAddress)
         continue;

      preambleAddress = searchAddress;

      variableSize = dataBlock->AsBlock.AsDataPreamble.VariableSize;
      readCrc = dataBlock->AsBlock.AsDataPreamble.Crc;
      isDeleted = dataBlock->AsBlock.AsDataPreamble.IsDeleted;

      readBlocksCount = VariableBlocksCount(variableSize);
      computedCrc = ComputeDataPreambleBlockCrc(dataBlock);
      computedBlocksCount = VariableBlocksCount(0);

      if(isDeleted != true && readBlocksCount != VariableBlocksCount(0))
      {
         for(;;)
         {
            searchAddress = NextBlockAddress(searchAddress);

            __RETURN_ERROR_IF_ANY(ReadBlockToBuffer(handle, searchAddress));

            if(IsBlockOfType(dataBlock, BLOCK_DESCRIPTOR_VARIABLE_DATA) != true)
               break;

            computedCrc = UpdateCrcByDataBlock(dataBlock, computedCrc);
            computedBlocksCount++;
         }
      }
   } while(computedCrc != readCrc || computedBlocksCount != readBlocksCount);

   handle->Dynamic.VariableDataCache = (__typeof__(handle->Dynamic.VariableDataCache))
   {
      .VariableAddress = variableAddress,
      .MemoryAddress = preambleAddress,
      .VariableSize = variableSize,
      .IsDeleted = isDeleted
   };

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   FlashFileSystemSDeviceSectorDynamicData *receivingSector = AnotherSector(handle, handle->Dynamic.ActiveSector);
   FlashFileSystemSDeviceState operationState;
   size_t blocksCount;
   intptr_t address;

   for(FlashFileSystemSDeviceAddress i = 0; i <= handle->Constant->MaxUsedAddress; i++)
   {
      operationState = CachePreambleBlockDataWithAddress(handle, i);

      if(operationState == FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR)
         continue;

      __RETURN_ERROR_IF_ANY(operationState);

      if(handle->Dynamic.VariableDataCache.IsDeleted == true)
         continue;

      blocksCount = VariableBlocksCount(handle->Dynamic.VariableDataCache.VariableSize);
      address = handle->Dynamic.VariableDataCache.MemoryAddress;

      while(blocksCount-- > 0)
      {
         address = NextBlockAddress(address);
         __RETURN_ERROR_IF_ANY(ReadBlockToBuffer(handle, address));
         __RETURN_ERROR_IF_ANY(WriteBlockFromBuffer(handle, receivingSector));
      }
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}
