#pragma once

#include <memory.h>

#include "../Inc/SimpleFsSDevice/public.h"

#define CREATE_SIMPLE_FS_APPLICATION(sectorSize)                                                                       \
   char memorySectors[2][(sectorSize)] = {0,};                                                                         \
   SectorsPtr = memorySectors[0];                                                                                      \
   SectorSize = sectorSize;                                                                                            \
   SimpleFsSDeviceSector sector$0 = { &(SectorContext){ 0 }, (sectorSize) };                                           \
   SimpleFsSDeviceSector sector$1 = { &(SectorContext){ 1 }, (sectorSize) };                                           \
   SDEVICE_INIT_DATA(SimpleFs) init =                                                                                  \
   {                                                                                                                   \
      .ReadUInt64 = ReadUInt64,                                                                                        \
      .WriteUInt64 = WriteUInt64,                                                                                      \
      .EraseSector = EraseSectorCallback,                                                                              \
      .Sector$0 = sector$0,                                                                                            \
      .Sector$1 = sector$1,                                                                                            \
      .IsMemoryErasingToZero = true                                                                                    \
   };

#define SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(SimpleFs))))

typedef struct
{
   size_t SectorIndex;
} SectorContext;

extern char *SectorsPtr;
extern size_t SectorSize;

void ReadUInt64(SDEVICE_HANDLE(SimpleFs)     *handle,
                const SimpleFsSDeviceSector  *sector,
                uintptr_t                    address,
                uint64_t                     *value);

void WriteUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                 const SimpleFsSDeviceSector *sector,
                 uintptr_t                   address,
                 uint64_t                    value);


void EraseSectorCallback(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

bool IsSectorEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2);
