#include "application.h"

static char *GlobalSectorsPtr;
static size_t GlobalSectorSize;

static uint16_t *GlobalBadBlocksNumbersArrayPtrSector$0;
static uint16_t *GlobalBadBlocksNumbersArrayPtrSector$1;

static size_t GlobalBadBlocksNumbersArraySizeSector$0;
static size_t GlobalBadBlocksNumbersArraySizeSector$1;

void SetGlobalBadBlocksNumbersArrayPtr(uint16_t *ptr, uint8_t sectorNumber)
{
   if(sectorNumber)
      GlobalBadBlocksNumbersArrayPtrSector$1 = ptr;
   else
      GlobalBadBlocksNumbersArrayPtrSector$0 = ptr;
}

void SetGlobalBadBlocksNumbersArraySize(size_t size, uint8_t sectorNumber)
{
   if(sectorNumber)
      GlobalBadBlocksNumbersArraySizeSector$1 = size;
   else
      GlobalBadBlocksNumbersArraySizeSector$0 = size;
}

uint16_t* GetGlobalBadBlocksNumbersArrayPtr(uint8_t sectorNumber)
{
   if(sectorNumber)
      return GlobalBadBlocksNumbersArrayPtrSector$1;

   return GlobalBadBlocksNumbersArrayPtrSector$0;
}

size_t GetGlobalBadBlocksNumbersArraySize(uint8_t sectorNumber)
{
   if(sectorNumber)
      return GlobalBadBlocksNumbersArraySizeSector$1;

   return GlobalBadBlocksNumbersArraySizeSector$0;
}

size_t GetGlobalSectorSize(void)
{
   return GlobalSectorSize;
}

void SetGlobalSectorSize(size_t size)
{
   GlobalSectorSize = size;
}

char* GetGlobalSectorsPtr(void)
{
   return GlobalSectorsPtr;
}

void SetGlobalSectorsPtr(char *ptr)
{
   GlobalSectorsPtr = ptr;
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
   uint8_t sectorIndex = ((SectorContext*)sector->Context)->SectorIndex;
   char *dataDstPtr = (SectorsPtr + SectorSize*sectorIndex + address);
   memcpy(dataDstPtr, &value, sizeof(value));

   if(GetGlobalBadBlocksNumbersArrayPtr(sectorIndex) != NULL)
   {
      uint16_t *ptrToArray = GetGlobalBadBlocksNumbersArrayPtr(sectorIndex);
      uint16_t amountOfElements = GetGlobalBadBlocksNumbersArraySize(sectorIndex) / sizeof(uint16_t);
      uint16_t numberOfBlock = address / sizeof(Block);

      for (uint16_t i = 0; i < amountOfElements; i++)
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

Block CreateHeaderBlock(SectorState state, bool isMemoryErasingToZero)
{
   HeaderBlock block =
   {
      .Type = BLOCK_TYPE_HEADER,
      .SectorState = state,
      .FsVersion = SIMPLE_FS_SDEVICE_CORE_VERSION,
      .Padding = isMemoryErasingToZero ? 0 : UINT8_MAX,
   };

   uint8_t *blockdata = ((ServiceBlock)block).BlockData;
   size_t size = sizeof(((ServiceBlock)block).BlockData);
   block.BlockCrc = TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, blockdata, size);

   return (Block)block;
}
