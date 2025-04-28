#pragma once

#include "SimpleFsSDevice/public.h"

typedef struct
{
   char  *Data;
   size_t SectorSize;
} MockSectors;

typedef struct
{
   size_t SectorIdx;
} SectorContext;

void GetMockSectors(MockSectors *sectors);
void SetMockSectors(MockSectors sectors);

void MockReadUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                    const SimpleFsSDeviceSector *sector,
                    uintptr_t                    address,
                    uint64_t                    *value);

void MockWriteUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                     const SimpleFsSDeviceSector *sector,
                     uintptr_t                    address,
                     uint64_t                     value);

void MockEraseSector(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);
