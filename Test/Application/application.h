#pragma once

#include <memory.h>

#include "../Src/private.h"
#include "../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#define CREATE_SIMPLE_FS_APPLICATION(sectorSize)                                                                       \
   char memorySectors[2][(sectorSize)] = {0,};                                                                         \
   SetGlobalSectorSize(sectorSize);                                                                                    \
   SetGlobalSectorsPtr(memorySectors[0]);                                                                              \
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

size_t GetGlobalSectorSize(void);

void SetGlobalSectorSize(size_t size);

char* GetGlobalSectorsPtr(void);

void SetGlobalSectorsPtr(char* ptr);

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

Block CreateHeaderBlock(SectorState state, bool isMemoryErasingToZero);
