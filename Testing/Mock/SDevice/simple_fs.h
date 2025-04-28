#pragma once

#include "Bindings/Memory/io.h"
#include "Bindings/Memory/bad_block.h"

#define SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE __attribute__((cleanup(SDEVICE_DISPOSE_HANDLE(SimpleFs))))

#define MOCK_SECTORS_DATA(name) name##MockSectorsData
#define SIMPLE_FS_PRIMARY_SECTOR(name) name##SimpleFsPrimarySector
#define SIMPLE_FS_AUXILIARY_SECTOR(name) name##SimpleFsAuxiliarySector
#define SIMPLE_FS_INIT(name) name##SimpleFsInit

#define CREATE_SIMPLE_FS_INIT(name, sector_size)                                                                       \
   char MOCK_SECTORS_DATA(name)[2][(sector_size)] = { {0}, {0} };                                                      \
   SetMockSectors((MockSectors){ .Data = (char *)MOCK_SECTORS_DATA(name), .SectorSize = (sector_size) });              \
                                                                                                                       \
   SimpleFsSDeviceSector SIMPLE_FS_PRIMARY_SECTOR(name) =                                                              \
   {                                                                                                                   \
      .Context = &(SectorContext){ 0 },                                                                                \
      .Size    = (sector_size)                                                                                         \
   };                                                                                                                  \
                                                                                                                       \
   SimpleFsSDeviceSector SIMPLE_FS_AUXILIARY_SECTOR(name) =                                                            \
   {                                                                                                                   \
      .Context = &(SectorContext){ 1 },                                                                                \
      .Size    = (sector_size)                                                                                         \
   };                                                                                                                  \
                                                                                                                       \
   SDEVICE_INIT_DATA(SimpleFs) SIMPLE_FS_INIT(name) =                                                                  \
   {                                                                                                                   \
      .ReadUInt64            = MockReadUInt64,                                                                         \
      .WriteUInt64           = MockWriteUInt64,                                                                        \
      .EraseSector           = MockEraseSector,                                                                        \
      .PrimarySector         = SIMPLE_FS_PRIMARY_SECTOR(name),                                                         \
      .AuxiliarySector       = SIMPLE_FS_AUXILIARY_SECTOR(name),                                                       \
      .IsMemoryErasingToZero = true                                                                                    \
   };

bool IsSimpleFsSectorsEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2);
