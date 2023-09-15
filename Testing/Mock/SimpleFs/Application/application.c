#include "application.h"
#include "../Data/BadBlock/block.h"

#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <memory.h>

typedef struct
{
   char  *GlobalSectorsPtr;
   size_t GlobalSectorSize;
} GlobalSectors;

static GlobalSectors Sectors;

void SetGlobalSectors(char *ptr, size_t size)
{
   Sectors.GlobalSectorSize = size;
   Sectors.GlobalSectorsPtr = ptr;
}

size_t GetGlobalSectorSize(void)
{
   return Sectors.GlobalSectorSize;
}

char* GetGlobalSectorsPtr(void)
{
   return Sectors.GlobalSectorsPtr;
}

void ReadUInt64(SDEVICE_HANDLE(SimpleFs)     *handle,
                const SimpleFsSDeviceSector  *sector,
                uintptr_t                     address,
                uint64_t                     *value)
{
   char *SectorsPtr = GetGlobalSectorsPtr();
   size_t SectorSize = GetGlobalSectorSize();
   SectorContext *sectorContext = sector->Context;
   char *dataSrcPtr = (SectorsPtr + SectorSize*sectorContext->SectorIndex + address);
   memcpy(value, dataSrcPtr, sizeof(*value));
}

void WriteUInt64(SDEVICE_HANDLE(SimpleFs)       *handle,
                 const SimpleFsSDeviceSector    *sector,
                 uintptr_t                       address,
                 uint64_t                        value)
{
   char *SectorsPtr = GetGlobalSectorsPtr();
   size_t SectorSize = GetGlobalSectorSize();
   size_t sectorIndex = ((SectorContext*)sector->Context)->SectorIndex;
   char *dataDstPtr = (SectorsPtr + SectorSize*sectorIndex + address);
   memcpy(dataDstPtr, &value, sizeof(value));

   if(GetBadBlocksNumbersArrayPtr(sectorIndex) != NULL)
   {
      size_t *ptrToArray = GetBadBlocksNumbersArrayPtr(sectorIndex);
      size_t amountOfElements = GetBadBlocksNumbersArraySize(sectorIndex) / sizeof(size_t);
      size_t numberOfBlock = address / sizeof(Block);

      for (size_t i = 0; i < amountOfElements; i++)
      {
         if(*(ptrToArray + i) == numberOfBlock)
         {
            memset(dataDstPtr, '*', sizeof(value));
            break;
         }
      }
   }
}

void EraseSectorCallback(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector)
{
   char *SectorsPtr = GetGlobalSectorsPtr();
   size_t SectorSize = GetGlobalSectorSize();
   SectorContext *sectorContext = sector->Context;
   memset((SectorsPtr + SectorSize*sectorContext->SectorIndex) , 0x00, SectorSize);
}

bool IsSectorEquial(const SimpleFsSDeviceSector *sector1, const SimpleFsSDeviceSector *sector2)
{
   return (sector1->Context == sector2->Context && sector1->Size == sector2->Size);
}
