#include "Processing/operations.h"
#include "Processing/Base/iterator.h"

#include <memory.h>

static FlashFileSystemStatus WriteVariable(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemAddress address,
                                          const void *data,
                                          size_t size,
                                          bool delete)
{
   /* check if there is enough free space and initiate transfer if not */
   if(EmptyBlocksCount(handle->Dynamic.ActiveIterator) < VariableBlocksCount(size))
   {
      __RETURN_ERROR_IF_ANY(TransferSectors(handle));

      /* there is still not enough space after transfer -> out of memory */
      if(EmptyBlocksCount(handle->Dynamic.ActiveIterator) < VariableBlocksCount(size))
      {
         SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_OUT_OF_MEMORY);
         return FLASH_FILE_SYSTEM_STATUS_OUT_OF_MEMORY_ERROR;
      }
   }

   /* create preamble block */
   FileSystemBlock block;
   block.AsBlock = (struct CommonBlock)
   {
      .Descriptor = BLOCK_DESCRIPTOR_PREAMBLE,
      .AsDataPreamble = (struct DataPreambleBlock)
      {
         .IsDeleted = delete,
         .VariableSize = size,
         .Address = address,
         .Padding = __BYTE_EMPTY_VALUE
      }
   };

   CrcType crc = ComputeDataPreambleBlockCrc(&block);
   crc = UpdateCrc16(data, size, crc);
   block.AsBlock.AsDataPreamble.Crc = crc;

   /* write preamble block */
   __RETURN_ERROR_IF_ANY(WriteForwardToCurrentBlock(handle, handle->Dynamic.ActiveIterator, &block));

   /* if it's delete write, there is no other blocks */
   if(delete == true)
      return FLASH_FILE_SYSTEM_STATUS_OK;

   block.AsBlock.Descriptor = BLOCK_DESCRIPTOR_VARIABLE_DATA;

   /* make and write data blocks */
   while(size > 0)
   {
      size_t blockWriteSize = __MIN(size, sizeof(block.AsBlock.AsData.Data));
      memcpy(block.AsBlock.AsData.Data, data, blockWriteSize);

      /* set all unused data bytes to empty value if block has such */
      if(blockWriteSize < sizeof(block.AsBlock.AsData.Data))
      {
         memset(&block.AsBlock.AsData.Data[blockWriteSize],
                __BYTE_EMPTY_VALUE,
                sizeof(block.AsBlock.AsData.Data) - blockWriteSize);
      }

      __RETURN_ERROR_IF_ANY(WriteForwardToCurrentBlock(handle, handle->Dynamic.ActiveIterator, &block));

      data += blockWriteSize;
      size -= blockWriteSize;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

static FlashFileSystemStatus ClearMemoryState(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_CORRUPTED_STATE);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SECTORS_COUNT; i++)
      __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[i], HEADER_STATE_ERASED));

   __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[0], HEADER_STATE_ACTIVE));
   handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[0];

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

FlashFileSystemStatus FlashFileSystemProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   SDeviceAssert(handle != NULL);

   SectorInitialState sectorsState[__FLASH_FILE_SYSTEM_SECTORS_COUNT];

   /* read and partially preprocess sectors initial states */
   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SECTORS_COUNT; i++)
   {
      __RETURN_ERROR_IF_ANY(GetSectorInitialState(handle, &handle->Dynamic.Iterators[i], &sectorsState[i]));

      /* sector is empty, set it's header state to ERASED */
      if(sectorsState[i].IsEmpty == true)
      {
         __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, &handle->Dynamic.Iterators[i], HEADER_STATE_ERASED));
         sectorsState[i].HeaderState = HEADER_STATE_ERASED;
         continue;
      }

      /* sector has no valid header state, format it to ERASED state */
      if(sectorsState[i].HasValidHeaderState != true)
      {
         __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[i], HEADER_STATE_ERASED));
         sectorsState[i].HeaderState = HEADER_STATE_ERASED;
         continue;
      }
   }

   /* process sector header block state combinations */
   switch(sectorsState[0].HeaderState)
   {
      case HEADER_STATE_ACTIVE:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_TRANSFER_ONGOING:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[1], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[0];
               __RETURN_ERROR_IF_ANY(TransferSectors(handle));
               break;

            case HEADER_STATE_TRANSFER_END:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[0], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[1];
               __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, handle->Dynamic.ActiveIterator, HEADER_STATE_ACTIVE));
               break;

            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[0];
               break;

            /* invalid state */
            case HEADER_STATE_ACTIVE:
               return ClearMemoryState(handle);

            default:
               SDeviceAssert(false);
               return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_TRANSFER_ONGOING:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[0], HEADER_STATE_ERASED));
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[1];
               __RETURN_ERROR_IF_ANY(TransferSectors(handle));
               break;

            /* invalid states */
            case HEADER_STATE_TRANSFER_ONGOING:
               /* fall through */
            case HEADER_STATE_TRANSFER_END:
               /* fall through */
            case HEADER_STATE_ERASED:
               return ClearMemoryState(handle);

            default:
               SDeviceAssert(false);
               return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_TRANSFER_END:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               __RETURN_ERROR_IF_ANY(FormatSectorToState(handle, &handle->Dynamic.Iterators[1], HEADER_STATE_ERASED));
               /* fall through */
            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[0];
               __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, handle->Dynamic.ActiveIterator, HEADER_STATE_ACTIVE));
               break;

            /* invalid states */
            case HEADER_STATE_TRANSFER_ONGOING:
               /* fall through */
            case HEADER_STATE_TRANSFER_END:
               return ClearMemoryState(handle);

            default:
               SDeviceAssert(false);
               return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
         }
         break;

      case HEADER_STATE_ERASED:
         switch(sectorsState[1].HeaderState)
         {
            case HEADER_STATE_ACTIVE:
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[1];
               break;

            case HEADER_STATE_TRANSFER_END:
               /* fall through */
            case HEADER_STATE_ERASED:
               handle->Dynamic.ActiveIterator = &handle->Dynamic.Iterators[1];
               __RETURN_ERROR_IF_ANY(SetSectorHeaderState(handle, handle->Dynamic.ActiveIterator, HEADER_STATE_ACTIVE));
               break;

            /* invalid states */
            case HEADER_STATE_TRANSFER_ONGOING:
               return ClearMemoryState(handle);

            default:
               SDeviceAssert(false);
               return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
         }
         break;

      default:
         SDeviceAssert(false);
         return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

FlashFileSystemStatus FlashFileSystemGetVariableSize(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                    FlashFileSystemAddress address,
                                                    size_t *size)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(size != NULL);

   __RETURN_ERROR_IF_ANY(MoveVariableDataToCache(handle, address));

   if(handle->Dynamic.VariableDataCache.IsDeleted == true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   *size = handle->Dynamic.VariableDataCache.Size;

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

FlashFileSystemStatus FlashFileSystemRead(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                         FlashFileSystemAddress address,
                                         size_t size,
                                         void *data)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(data != NULL);

   FileSystemBlock block;

   __RETURN_ERROR_IF_ANY(MoveVariableDataToCache(handle, address));

   if(handle->Dynamic.VariableDataCache.IsDeleted == true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   if(size > handle->Dynamic.VariableDataCache.Size)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRONG_VARIABLE_SIZE);
      return FLASH_FILE_SYSTEM_STATUS_VALUE_SIZE_ERROR;
   }

   /* data begins right after preamble block */
   SeekReadCursor(handle->Dynamic.ActiveIterator, NextBlockAddress(handle->Dynamic.VariableDataCache.MemoryAddress));

   while(size > 0)
   {
      __RETURN_ERROR_IF_ANY(ReadForwardFromCurrentBlock(handle, handle->Dynamic.ActiveIterator, &block));

      size_t blockReadSize = __MIN(size, sizeof(block.AsBlock.AsData.Data));
      memcpy(data, block.AsBlock.AsData.Data, blockReadSize);

      data += blockReadSize;
      size -= blockReadSize;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

FlashFileSystemStatus FlashFileSystemWrite(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                          FlashFileSystemAddress address,
                                          size_t size,
                                          const void *data)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(data != NULL);

   return WriteVariable(handle, address, data, size, false);
}

FlashFileSystemStatus FlashFileSystemDelete(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemAddress address)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);

   return WriteVariable(handle, address, NULL, 0, true);
}
