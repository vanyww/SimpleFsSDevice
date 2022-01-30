#include "Processing/Base/block.h"

FlashFileSystemStatus FlashFileSystemProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *);
void InvalidateVariableDataCache(__SDEVICE_HANDLE(FlashFileSystem) *);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->TryWrite != NULL);
   SDeviceAssert(handle->Constant->TryRead != NULL);
   SDeviceAssert(handle->Constant->TryEraseSector != NULL);
   SDeviceAssert(handle->Constant->MaxUsedAddress <= __FLASH_FILE_SYSTEM_MAX_ADDRESS);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SECTORS_COUNT; i++)
      handle->Dynamic.Iterators[i].Sector = &handle->Constant->Sectors[i];

   InvalidateVariableDataCache(handle);
   if(FlashFileSystemProcessInitialState(handle) != FLASH_FILE_SYSTEM_STATUS_OK)
      return;

   handle->IsInitialized = true;
}
