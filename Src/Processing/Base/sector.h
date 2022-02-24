#include "block.h"

static inline uintptr_t SectorLastBlockAddress(const FlashFileSystemSector *sector)
{
   return PreviousBlockAddress(sector->StartAddress + sector->Size);
}

static inline uintptr_t SectorFirstBlockAddress(const FlashFileSystemSector *sector)
{
   return sector->StartAddress;
}

static inline FlashFileSystemStatus EraseSector(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                const FlashFileSystemSector *sector)
{
   if(handle->Init.TryEraseSector(handle, sector) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_ERASE_FAIL);
      return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}
