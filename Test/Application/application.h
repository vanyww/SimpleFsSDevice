#pragma once

#include <memory.h>

#include "../Inc/SimpleFsSDevice/public.h"

typedef struct
{
   size_t SectorIndex;
} SectorContext;

extern char *SectorsPtr;
extern size_t SectorSize;

void ReadUInt64(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector,
                uintptr_t address, uint64_t *value);

void WriteUInt64(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector,
                 uintptr_t address, uint64_t value);

void Erase_Sector(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

bool IsSectorEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2);

#define _createApplication(sectorSize)                                          \
char memorySectors[2][(sectorSize)];                                            \
SectorsPtr = &memorySectors[0][0];                                              \
SectorSize = sectorSize;                                                        \
SimpleFsSDeviceSector sector$0 = { &(SectorContext){ 0 }, (sectorSize)};        \
SimpleFsSDeviceSector sector$1 = { &(SectorContext){ 1 }, (sectorSize)};        \
SDEVICE_INIT_DATA(SimpleFs) init = { ReadUInt64, WriteUInt64,                   \
      Erase_Sector, sector$0, sector$1, true }

#define _cleanup __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(SimpleFs))))
