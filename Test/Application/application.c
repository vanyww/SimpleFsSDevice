#include "application.h"

char *SectorsPtr;
size_t SectorSize;

void ReadUInt64(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector,
                uintptr_t address, uint64_t *value)
{
   SectorContext *sectorContext = sector->Context;
   char* dataSrcPtr = (SectorsPtr + SectorSize*sectorContext->SectorIndex + address);
   memcpy(value, dataSrcPtr, sizeof(*value));
}

void WriteUInt64(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector,
                 uintptr_t address, uint64_t value)
{
   SectorContext *sectorContext = sector->Context;
   char* dataDstPtr = (SectorsPtr + SectorSize*sectorContext->SectorIndex + address);
   memcpy(dataDstPtr, &value, sizeof(value));
}

void Erase_Sector(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector)
{
   SectorContext *sectorContext = sector->Context;
   memset((SectorsPtr + SectorSize*sectorContext->SectorIndex) , 0x00, SectorSize);
}

bool IsSectorEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2)
{
   return (sector1->Context == sector2->Context && sector1->Size == sector2->Size);
}
