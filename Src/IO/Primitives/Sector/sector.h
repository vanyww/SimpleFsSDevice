#pragma once

#include "../../../private.h"
#include "../Block/Base/DataTypes/block.h"

#include "SDeviceCore/errors.h"

#define SIMPLE_FS_SDEVICE_MIN_SECTOR_SIZE (6U * sizeof(uint64_t)) /* 6 blocks: 4 headers, 1 data, 1 data tag */

static inline bool HasSectorValidSize(const Sector *sector)
{
   SDeviceDebugAssert(sector != NULL);

   return sector->Size >= SIMPLE_FS_SDEVICE_MIN_SECTOR_SIZE;
}

static inline bool CanAccessSectorUInt64ByAddress(const Sector *sector, uintptr_t address)
{
   SDeviceDebugAssert(sector != NULL);
   SDeviceDebugAssert(HasSectorValidSize(sector));

   return address <= sector->Size - sizeof(uint64_t);
}

static inline size_t GetSectorTotalLength(const Sector *sector)
{
   SDeviceDebugAssert(sector != NULL);

   return sector->Size / sizeof(Block);
}

static inline void EraseSector(ThisHandle *handle, const Sector *sector)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(sector != NULL);

   handle->Init.EraseSector(handle, sector);
}

static inline void ReadSectorUInt64(ThisHandle *handle, const Sector *sector, uintptr_t address, uint64_t *value)
{
   SDeviceDebugAssert(value != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(sector != NULL);
   SDeviceDebugAssert(CanAccessSectorUInt64ByAddress(sector, address));

   handle->Init.ReadUInt64(handle, sector, address, value);
}

static inline void WriteSectorUInt64(ThisHandle *handle, const Sector *sector, uintptr_t address, uint64_t value)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(sector != NULL);
   SDeviceDebugAssert(CanAccessSectorUInt64ByAddress(sector, address));

   handle->Init.WriteUInt64(handle, sector, address, value);
}
