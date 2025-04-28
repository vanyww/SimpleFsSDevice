#include "io.h"
#include "bad_block.h"

#include "../../../../../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <memory.h>

MockSectors ThisSectors;

void GetMockSectors(MockSectors *sectors)
{
   *sectors = ThisSectors;
}

void SetMockSectors(MockSectors sectors)
{
   ThisSectors = sectors;
}

void MockReadUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                    const SimpleFsSDeviceSector *sector,
                    uintptr_t                    address,
                    uint64_t                    *value)
{
   size_t SectorSize = ThisSectors.SectorSize;
   char *SectorsPtr = ThisSectors.Data;
   SectorContext *sectorContext = sector->Context;
   char *dataSrcPtr = (SectorsPtr + SectorSize*sectorContext->SectorIdx + address);
   memcpy(value, dataSrcPtr, sizeof(*value));
}

void MockWriteUInt64(SDEVICE_HANDLE(SimpleFs)    *handle,
                     const SimpleFsSDeviceSector *sector,
                     uintptr_t                    address,
                     uint64_t                     value)
{
   size_t SectorSize = ThisSectors.SectorSize;
   char *SectorsPtr = ThisSectors.Data;
   size_t sectorIndex = ((SectorContext*)sector->Context)->SectorIdx;
   char *dataDstPtr = (SectorsPtr + SectorSize*sectorIndex + address);
   memcpy(dataDstPtr, &value, sizeof(value));

   size_t badBlocksIdxsCount;
   size_t *badBlocksIdxs;
   MockBadBlocks badBlocks;
   GetMockBadBlocks(&badBlocks);

   if(sectorIndex == 0)
   {
      badBlocksIdxs = badBlocks.PrimarySectorBadBlocksIdxs;
      badBlocksIdxsCount = badBlocks.PrimarySectorBadBlocksIdxsCount;
   }
   else
   {
      badBlocksIdxs = badBlocks.AuxiliarySectorBadBlocksIdxs;
      badBlocksIdxsCount = badBlocks.AuxiliarySectorBadBlocksIdxsCount;
   }

   if(badBlocksIdxsCount > 0)
   {
      size_t blockIdx = address / sizeof(Block);

      for(size_t i = 0; i < badBlocksIdxsCount; i++)
      {
         if(*(badBlocksIdxs + i) == blockIdx)
         {
            memset(dataDstPtr, '*', sizeof(value));
            break;
         }
      }
   }
}

void MockEraseSector(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector)
{
   size_t SectorSize = ThisSectors.SectorSize;
   char *SectorsPtr = ThisSectors.Data;
   SectorContext *sectorContext = sector->Context;
   memset((SectorsPtr + SectorSize*sectorContext->SectorIdx) , 0x00, SectorSize);
}
