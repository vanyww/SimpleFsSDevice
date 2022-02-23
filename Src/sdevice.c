#include "Processing/Base/block.h"

FlashFileSystemStatus FlashFileSystemProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *);
void InvalidateVariableDataCache(__SDEVICE_HANDLE(FlashFileSystem) *);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Init.TryWrite != NULL);
   SDeviceAssert(handle->Init.TryRead != NULL);
   SDeviceAssert(handle->Init.TryEraseSector != NULL);
   SDeviceAssert(handle->Init.MaxUsedAddress <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SECTORS_COUNT; i++)
      handle->Runtime.Iterators[i].Sector = &handle->Init.Sectors[i];

   InvalidateVariableDataCache(handle);
   if(FlashFileSystemProcessInitialState(handle) != FLASH_FILE_SYSTEM_STATUS_OK)
      return;

   handle->IsInitialized = true;
}
