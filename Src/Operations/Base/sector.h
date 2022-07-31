#pragma once

#include "../../Operations/Base/block.h"

static inline uintptr_t GetSectorStartAddress(const SimpleFsSDeviceSector *sector)
{
   return sector->Address;
}

static inline uintptr_t GetSectorEndAddress(const SimpleFsSDeviceSector *sector)
{
   return GetPreviousBlockAddress(sector->Address + sector->Size);
}

static inline void EraseSector(__SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector)
{
   if(handle->Init.TryEraseSector(handle, sector) != true)
      SDeviceThrow(handle, SIMPLE_FS_SDEVICE_EXCEPTION_ERASE_FAIL);
}
