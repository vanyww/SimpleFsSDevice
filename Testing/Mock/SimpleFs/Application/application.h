#pragma once

#include "SimpleFsSDevice/public.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/Enumerations/sector_state.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#define MEMORY_SECTORS(name) _##name##MemorySectors
#define SECTOR$0(name) _##name##Sector0
#define SECTOR$1(name) _##name##Sector1
#define INIT(name) _##name##InitData

#define CREATE_SIMPLE_FS_APPLICATION(sector_size, name)                                                                \
   char MEMORY_SECTORS(name)[2][(sector_size)] = { {0}, {0} };                                                         \
   SetGlobalSectors(MEMORY_SECTORS(name)[0], sector_size);                                                             \
                                                                                                                       \
   SimpleFsSDeviceSector SECTOR$0(name) =                                                                              \
   {                                                                                                                   \
      .Context = &(SectorContext){ 0 },                                                                                \
      .Size = (sector_size)                                                                                            \
   };                                                                                                                  \
   SimpleFsSDeviceSector SECTOR$1(name) =                                                                              \
   {                                                                                                                   \
      .Context = &(SectorContext){ 1 },                                                                                \
      .Size = (sector_size)                                                                                            \
   };                                                                                                                  \
   SDEVICE_INIT_DATA(SimpleFs) INIT(name) =                                                                            \
   {                                                                                                                   \
      .ReadUInt64 = ReadUInt64,                                                                                        \
      .WriteUInt64 = WriteUInt64,                                                                                      \
      .EraseSector = EraseSectorCallback,                                                                              \
      .Sector$0 = SECTOR$0(name),                                                                                      \
      .Sector$1 = SECTOR$1(name),                                                                                      \
      .IsMemoryErasingToZero = true                                                                                    \
   };

#define SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(SimpleFs))))

typedef struct
{
   size_t SectorIndex;
} SectorContext;

void SetGlobalSectors(char *ptr, size_t size);

size_t GetGlobalSectorSize(void);

char* GetGlobalSectorsPtr(void);




void ReadUInt64(SDEVICE_HANDLE(SimpleFs)     *handle,
                const SimpleFsSDeviceSector  *sector,
                uintptr_t                     address,
                uint64_t                     *value);

void WriteUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                 const SimpleFsSDeviceSector *sector,
                 uintptr_t                    address,
                 uint64_t                     value);


void EraseSectorCallback(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

bool IsSectorEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2);

