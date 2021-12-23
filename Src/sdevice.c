#include "Processing/Base/block.h"

FlashFileSystemSDeviceState FlashFileSystemSDeviceProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *);
void InvalidateVariableDataCache(__SDEVICE_HANDLE(FlashFileSystem) *);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem, handle)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(handle->Constant != NULL);
   SDeviceAssert(handle->IsInitialized == false);
   SDeviceAssert(handle->Constant->TryWriteToFlash != NULL);
   SDeviceAssert(handle->Constant->TryReadFromFlash != NULL);
   SDeviceAssert(handle->Constant->TryEraseFlashSector != NULL);
   SDeviceAssert(handle->Constant->MaxUsedAddress <= __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT; i++)
      handle->Dynamic.Iterators[i].Sector = &handle->Constant->Sectors[i];

   InvalidateVariableDataCache(handle);
   handle->IsInitialized = FlashFileSystemSDeviceProcessInitialState(handle) == FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}
