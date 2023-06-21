#include "application.h"

static char *GlobalSectorsPtr;
static size_t GlobalSectorSize;

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

void SetGlobalSectorsPtr(char* ptr)
{
   GlobalSectorsPtr = ptr;
}

void ReadUInt64(SDEVICE_HANDLE(SimpleFs)     *handle,
                const SimpleFsSDeviceSector  *sector,
                uintptr_t                    address,
                uint64_t                     *value)
{
   char *SectorsPtr = GetGlobalSectorsPtr();
   size_t SectorSize = GetGlobalSectorSize();
   SectorContext *sectorContext = sector->Context;
   char* dataSrcPtr = (SectorsPtr + SectorSize*sectorContext->SectorIndex + address);
   memcpy(value, dataSrcPtr, sizeof(*value));
}


void WriteUInt64(SDEVICE_HANDLE(SimpleFs)       *handle,
                 const SimpleFsSDeviceSector    *sector,
                 uintptr_t                      address,
                 uint64_t                       value)
{
   char *SectorsPtr = GetGlobalSectorsPtr();
   size_t SectorSize = GetGlobalSectorSize();
   SectorContext *sectorContext = sector->Context;
   char* dataDstPtr = (SectorsPtr + SectorSize*sectorContext->SectorIndex + address);
   memcpy(dataDstPtr, &value, sizeof(value));
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
         .Padding = ({isMemoryErasingToZero ? 0 : UINT8_MAX;})
      };

   uint8_t* blockdata = ((ServiceBlock)block).BlockData;
   size_t size = sizeof(((ServiceBlock)block).BlockData);
   block.BlockCrc = TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, blockdata, size);

   return (Block)block;
}
