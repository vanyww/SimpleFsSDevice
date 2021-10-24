#include "../Inc/FlashFileSystemSDevice/core.h"
#include "Processing/Base/block.h"

FlashFileSystemSDeviceState FlashFileSystemSDeviceProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem, handle)
{
   SDeviceAssert(handle->Constant->MaxUsedAddress <= __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT; i++)
      handle->Dynamic.Iterators[i].Sector = &handle->Constant.Sectors[i];

   /* set cache address to invalid value to mark it as invalid */
   handle->Dynamic.VariableDataCache.Address = __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS + 1;
   handle->IsInitialized = FlashFileSystemSDeviceProcessInitialState(handle);
}
