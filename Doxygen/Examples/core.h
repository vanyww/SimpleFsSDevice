#pragma once

#include "SimpleFsSDevice/public.h"

#include <memory.h>

/* two sectors of size 200 */
char MemorySectors[2][200];

typedef struct
{
   size_t SectorIndex;
} SectorContext;

void ReadUInt64(SDEVICE_HANDLE(SimpleFs) *handle,
                const SimpleFsSDeviceSector *sector,
                uintptr_t address,
                uint64_t *value)
{
   SectorContext *sectorContext = sector->Context;
   memcpy(value, &MemorySectors[sectorContext->SectorIndex][address], sizeof(*value));
}

void WriteUInt64(SDEVICE_HANDLE(SimpleFs) *handle,
                 const SimpleFsSDeviceSector *sector,
                 uintptr_t address,
                 uint64_t value)
{
   SectorContext *sectorContext = sector->Context;
   memcpy(&MemorySectors[sectorContext->SectorIndex][address], &value, sizeof(value));
}

void EraseSector(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector)
{
   SectorContext *sectorContext = sector->Context;
   memset(MemorySectors[sectorContext->SectorIndex], 0x00, sizeof(MemorySectors[sectorContext->SectorIndex]));
}

void main(void)
{
   SimpleFsSDeviceSector sector$0 = { &(SectorContext){ 0 }, sizeof(MemorySectors[0]) };
   SimpleFsSDeviceSector sector$1 = { &(SectorContext){ 1 }, sizeof(MemorySectors[1]) };
   SDEVICE_INIT_DATA(SimpleFs) init = { ReadUInt64, WriteUInt64, EraseSector, sector$0, sector$1, true };

   SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   uint16_t fileId = 0;


   char value$0[] = "hello there";
   SimpleFsSDeviceWriteFile(handle, fileId, value$0, sizeof(value$0));

   char value$1[] = "hello there, fellow kids";
   SimpleFsSDeviceWriteFile(handle, fileId, value$1, sizeof(value$1));

   size_t fileSize = SimpleFsSDeviceGetMaxFileSize(handle, fileId);
   SDeviceAssert(sizeof(value$1) == fileSize);

   char read$0[sizeof(value$0)];
   /* will read value$0 as it's the newest file version that has size less or equal to sizeof(read$0) */
   SimpleFsSDeviceReadFile(handle, fileId, read$0, sizeof(read$0));

   char read$1[sizeof(value$1)];
   /* will read value$1 as it's the newest file version that has size less or equal to sizeof(read$1) */
   SimpleFsSDeviceReadFile(handle, fileId, read$1, sizeof(read$1));

   SimpleFsSDeviceForceHistoryDeletion(handle);

   char read$3[sizeof(value$0)];
   /* will read nothing (return 0), as history (value$0) was deleted and value$1 is larger than sizeof(read$3) */
   SimpleFsSDeviceReadFile(handle, fileId, read$3, sizeof(read$3));

   /* no files are accessible after deletion, but value$1 version is still stored until next sector transfer */
   SimpleFsSDeviceDeleteFile(handle, fileId);

   /* memory fully cleared after format */
   SimpleFsSDeviceFormatMemory(handle);
}
