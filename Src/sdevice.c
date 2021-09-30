#include "../Inc/FlashFileSystemSDevice/core.h"
#include "Processing/Primitives/block.h"

#include <assert.h>

FlashFileSystemSDeviceState FlashFileSystemSDeviceProcessInitialState(__SDEVICE_HANDLE(FlashFileSystem) *);

__SDEVICE_INITIALIZE_INTERNALS_DECLARATION(FlashFileSystem, handle)
{
   static_assert(sizeof(FileSystemBlock) == __FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE);
   SDeviceAssert(handle->Constant->MaxUsedAddress <= __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS);

   for(size_t i = 0; i < __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT; i++)
      handle->Dynamic.Sectors[i].Sector = &handle->Constant->Sectors[i];

   handle->Dynamic.VariableDataCache.VariableAddress = __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS;

   return FlashFileSystemSDeviceProcessInitialState(handle) == FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}
