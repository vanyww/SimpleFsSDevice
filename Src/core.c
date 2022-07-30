#include "Operations/operations.h"
#include "Operations/Helpers/helpers.h"
#include "Operations/Base/iterator.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/heap.h"

#include <memory.h>

/**********************************************************************************************************************/

__SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFS, _init, _context, index)
{
   __SDEVICE_INIT_DATA(SimpleFs) *init = _init;

   SDeviceAssert(init != NULL);
   SDeviceAssert(init->WriteBlock != NULL);
   SDeviceAssert(init->ReadBlock != NULL);
   SDeviceAssert(init->EraseSector != NULL);

   __SDEVICE_HANDLE(SimpleFs) *handle = SDeviceMalloc(sizeof(__SDEVICE_HANDLE(SimpleFs)));

   handle->Header = (SDeviceHandleHeader){ _context, SIMPLE_FS_SDEVICE_STATUS_OK, index };
   handle->Init = *init;

   for(size_t i = 0; i < __SIMPLE_FS_SDEVICE_SECTORS_COUNT; i++)
      handle->Runtime.Iterators[i].Sector = &handle->Init.Sectors[i];

   handle->Runtime.FileDataCache.IsValid = false;
   SimpleFsSDeviceProcessInitialState(handle);

   return handle;
}

__SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFS, _handlePointer)
{
   __SDEVICE_HANDLE(SimpleFs) **handlePointer = _handlePointer;
   SDeviceFree(*handlePointer);
   *handlePointer = NULL;
}

/**********************************************************************************************************************/

bool SimpleFsSDeviceTryGetFileSize(__SDEVICE_HANDLE(SimpleFs) *handle, SimpleFsSDeviceAddress address, size_t *size)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(size != NULL);

   if(SimpleFsSDeviceTryMoveFileDataToCache(handle, address) != true ||
      handle->Runtime.FileDataCache.IsDeleted == true)
      return false;

   *size = handle->Runtime.FileDataCache.Size;

   return true;
}

bool SimpleFsSDeviceTryRead(__SDEVICE_HANDLE(SimpleFs) *handle, SimpleFsSDeviceAddress address, size_t size, void *data)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(data != NULL);

   Block block;

   if(SimpleFsSDeviceTryMoveFileDataToCache(handle, address) != true ||
      handle->Runtime.FileDataCache.IsDeleted == true)
      return false;

   if(size > handle->Runtime.FileDataCache.Size)
      return false;

   /* data begins right after preamble block */
   SeekIteratorReadCursor(handle->Runtime.ActiveIterator,
                          GetNextBlockAddress(handle->Runtime.FileDataCache.MemoryAddress));

   while(size > 0)
   {
      IteratorTryReadForward(handle, handle->Runtime.ActiveIterator, &block);

      size_t blockReadSize = __MIN(size, sizeof(block.AsBlock.AsData.Data));
      memcpy(data, block.AsBlock.AsData.Data, blockReadSize);

      data += blockReadSize;
      size -= blockReadSize;
   }

   return true;
}

bool FlashFileSystemWrite(__SDEVICE_HANDLE(SimpleFs) *handle,
                          SimpleFsSDeviceAddress address,
                          size_t size,
                          const void *data)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);
   SDeviceAssert(data != NULL);

   SimpleFsSDeviceWriteFile(handle, address, data, size, false);

   return true;
}

bool FlashFileSystemDelete(__SDEVICE_HANDLE(SimpleFs) *handle,
                           SimpleFsSDeviceAddress address)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == true);
   SDeviceAssert(address <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);

   SimpleFsSDeviceWriteFile(handle, address, NULL, 0, true);

   return true;
}
