#include "Processing/operations.h"

#include <memory.h>

static FlashFileSystemSDeviceState WriteVariable(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                 FlashFileSystemSDeviceAddress address,
                                                 const void *data,
                                                 size_t size,
                                                 bool delete)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;

   if(SectorFreeBlocksLeft(handle->Dynamic.ActiveSector) < VariableBlocksCount(size))
   {
      __RETURN_ERROR_IF_ANY(TransferSectors(handle));

      if(SectorFreeBlocksLeft(handle->Dynamic.ActiveSector) < VariableBlocksCount(size))
         return FLASH_FILE_SYSTEM_SDEVICE_STATE_OUT_OF_MEMORY_ERROR;
   }

   dataBlock->AsBlock = (struct CommonBlock)
   {
      .Descriptor = BLOCK_DESCRIPTOR_DATA_PREAMBLE,
      .AsDataPreamble = (struct DataPreambleBlock)
      {
         .IsDeleted = delete,
         .VariableSize = size,
         .Address = address,
         .Padding = __BYTE_EMPTY_VALUE
      }
   };

   CrcType crc = ComputeDataPreambleBlockCrc(dataBlock);
   crc = Crc16Update(data, size, crc);

   dataBlock->AsBlock.AsDataPreamble.Crc = crc;

   __RETURN_ERROR_IF_ANY(WriteBlockFromBuffer(handle, handle->Dynamic.ActiveSector));

   if(delete == true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;

   dataBlock->AsBlock.Descriptor = BLOCK_DESCRIPTOR_VARIABLE_DATA;
   size_t blockWriteSize;

   while(size > VariableBlocksCount(0))
   {
      blockWriteSize = __MIN(size, sizeof(dataBlock->AsBlock.AsData.Data));
      memcpy(dataBlock->AsBlock.AsData.Data, data, blockWriteSize);

      __RETURN_ERROR_IF_ANY(WriteBlockFromBuffer(handle, handle->Dynamic.ActiveSector));

      data += blockWriteSize;
      size -= blockWriteSize;
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState FlashFileSystemSDeviceProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   SectorInitialState sectorsState[__FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT];

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT; i++)
   {
      __RETURN_ERROR_IF_ANY(GetSectorState(handle, &handle->Constant->Sectors[i], &sectorsState[i]));
      handle->Dynamic.Sectors[i].FirstFreeBlockAddress = sectorsState[i].FirstEmptyBlockAddress;

      if(sectorsState[i].IsEmpty == true || sectorsState[i].HasValidState == false)
      {
         if(sectorsState[i].IsEmpty == true)
         {
            __RETURN_ERROR_IF_ANY(SetSectorState(handle, &handle->Dynamic.Sectors[i], HEADER_STATE_ERASED));
         }
         else
         {
            __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Sectors[i], HEADER_STATE_ERASED));
         }

         sectorsState[i].HeaderState = HEADER_STATE_ERASED;
      }
   }

   switch(sectorsState[0].HeaderState)
   {
      case HEADER_STATE_ACTIVE:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_TRANSFER_IN_PROGRESS:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Sectors[1], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[0];
               __RETURN_ERROR_IF_ANY(TransferSectors(handle));
               break;

            case HEADER_STATE_TRANSFER_END:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Sectors[0], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[1];
               __RETURN_ERROR_IF_ANY(SetSectorState(handle, handle->Dynamic.ActiveSector, HEADER_STATE_ACTIVE));
               break;

            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[0];
               break;

            default:
               return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_TRANSFER_IN_PROGRESS:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Sectors[0], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[1];
               __RETURN_ERROR_IF_ANY(TransferSectors(handle));
               break;

            default:
               return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_TRANSFER_END:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Sectors[1], HEADER_STATE_ERASED));
               /* fall through */

            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[0];
               __RETURN_ERROR_IF_ANY(SetSectorState(handle,  handle->Dynamic.ActiveSector, HEADER_STATE_ACTIVE));
               break;

            default:
               return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_ERASED:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[1];
               break;

            case HEADER_STATE_TRANSFER_END:
               /* fall through */

            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveSector = &handle->Dynamic.Sectors[1];
               __RETURN_ERROR_IF_ANY(SetSectorState(handle, handle->Dynamic.ActiveSector, HEADER_STATE_ACTIVE));
               break;

            default:
               return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;
         }
         break;

      default:
         SDeviceAssert(true);
         break;
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}


FlashFileSystemSDeviceState FlashFileSystemSDeviceGetVariableSize(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                  FlashFileSystemSDeviceAddress address,
                                                                  size_t *size)
{
   __RETURN_ERROR_IF_ANY(CachePreambleBlockDataWithAddress(handle, address));

   if(handle->Dynamic.VariableDataCache.IsDeleted == true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

   *size = handle->Dynamic.VariableDataCache.VariableSize;

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState FlashFileSystemSDeviceRead(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                       FlashFileSystemSDeviceAddress address,
                                                       size_t size,
                                                       void *data)
{
   FileSystemBlock *dataBlock = (FileSystemBlock *)handle->Dynamic.BlockBuffer;

   __RETURN_ERROR_IF_ANY(CachePreambleBlockDataWithAddress(handle, address));

   if(handle->Dynamic.VariableDataCache.IsDeleted == true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

   if(size > handle->Dynamic.VariableDataCache.VariableSize)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_SIZE_ERROR;

   intptr_t blockAddress = handle->Dynamic.VariableDataCache.MemoryAddress;
   size_t blockReadSize;

   while(size > 0)
   {
      blockAddress = NextBlockAddress(blockAddress);
      __RETURN_ERROR_IF_ANY(ReadBlockToBuffer(handle, blockAddress));

      blockReadSize = __MIN(size, sizeof(dataBlock->AsBlock.AsData.Data));
      memcpy(data, dataBlock->AsBlock.AsData.Data, blockReadSize);

      data += blockReadSize;
      size -= blockReadSize;
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}

FlashFileSystemSDeviceState FlashFileSystemSDeviceWrite(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                        FlashFileSystemSDeviceAddress address,
                                                        size_t size,
                                                        const void *data)
{
   return WriteVariable(handle, address, data, size, false);
}

FlashFileSystemSDeviceState FlashFileSystemSDeviceDelete(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                         FlashFileSystemSDeviceAddress address)
{
   return WriteVariable(handle, address, NULL, 0, true);
}
