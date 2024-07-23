#pragma once

#include "../../../private.h"
#include "../Block/Base/DataTypes/block.h"

#define SIMPLE_FS_SDEVICE_MIN_SECTOR_SIZE (6U * sizeof(uint64_t)) /* 6 blocks: 4 headers, 1 data, 1 data tag */

static inline bool HasSectorValidSize(const ThisSector *sector)
{
   return sector->Size >= SIMPLE_FS_SDEVICE_MIN_SECTOR_SIZE;
}

static inline bool CanAccessSectorUInt64ByAddress(const ThisSector *sector, uintptr_t address)
{
   return address <= sector->Size - sizeof(uint64_t);
}

static inline size_t GetSectorTotalLength(const ThisSector *sector)
{
   return sector->Size / sizeof(Block);
}

static inline void EraseSector(ThisHandle *handle, const ThisSector *sector)
{
   handle->Init->EraseSector(handle, sector);
}

static inline void ReadSectorUInt64(ThisHandle *handle, const ThisSector *sector, uintptr_t address, uint64_t *value)
{
   handle->Init->ReadUInt64(handle, sector, address, value);
}

static inline void WriteSectorUInt64(ThisHandle *handle, const ThisSector *sector, uintptr_t address, uint64_t value)
{
   handle->Init->WriteUInt64(handle, sector, address, value);
}
