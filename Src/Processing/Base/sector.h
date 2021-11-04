 #include "block.h"

static inline intptr_t SectorLastBlockAddress(const FlashFileSystemSDeviceSector *sector)
{
   return PreviousBlockAddress(sector->StartAddress + sector->Size);
}

static inline intptr_t SectorFirstBlockAddress(const FlashFileSystemSDeviceSector *sector)
{
   return sector->StartAddress;
}

static inline FlashFileSystemSDeviceState EraseSector(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                      const FlashFileSystemSDeviceSector *sector)
{
   if(handle->Constant->TryEraseFlashSector(handle, sector) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_SDEVICE_RUNTIME_IO_ERASE_MEMORY_ERROR);
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_SDEVICE_STATE_OK;
}
