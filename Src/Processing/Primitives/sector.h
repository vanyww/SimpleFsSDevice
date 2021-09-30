#include "block.h"

static inline intptr_t SectorLastBlockAddress(const FlashFileSystemSDeviceSector *sector)
{
   return PreviousBlockAddress(sector->StartAddress + sector->Size);
}

static inline size_t SectorFreeBlocksLeft(FlashFileSystemSDeviceSectorDynamicData *sector)
{
   if(sector->FirstFreeBlockAddress > SectorLastBlockAddress(sector->Sector))
      return 0;

   return 1 + (SectorLastBlockAddress(sector->Sector) - sector->FirstFreeBlockAddress) / BlocksSize(1);
}

static inline FlashFileSystemSDeviceSectorDynamicData * AnotherSector(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                      FlashFileSystemSDeviceSectorDynamicData *sector)
{
      return (sector == &handle->Dynamic.Sectors[0]) ? &handle->Dynamic.Sectors[1] : &handle->Dynamic.Sectors[0];
}

