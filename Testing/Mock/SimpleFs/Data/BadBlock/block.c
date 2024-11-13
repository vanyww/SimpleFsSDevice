#include "block.h"

static struct
{
   size_t *BadBlocksNumbersArrayPtrSector$0;
   size_t *BadBlocksNumbersArrayPtrSector$1;
   size_t  BadBlocksNumbersArraySizeSector$0;
   size_t  BadBlocksNumbersArraySizeSector$1;
} BadBlocksConfig;

void SetBadBlocksNumbers(size_t *ptr, size_t size, size_t sectorIndex)
{
   if(sectorIndex)
   {
      BadBlocksConfig.BadBlocksNumbersArrayPtrSector$1  = ptr;
      BadBlocksConfig.BadBlocksNumbersArraySizeSector$1 = size;
   }
   else
   {
      BadBlocksConfig.BadBlocksNumbersArrayPtrSector$0  = ptr;
      BadBlocksConfig.BadBlocksNumbersArraySizeSector$0 = size;
   }
}

size_t* GetBadBlocksNumbersArrayPtr(size_t sectorIndex)
{
   if(sectorIndex)
      return BadBlocksConfig.BadBlocksNumbersArrayPtrSector$1;

   return BadBlocksConfig.BadBlocksNumbersArrayPtrSector$0;
}

size_t GetBadBlocksNumbersArraySize(size_t sectorIndex)
{
   if(sectorIndex)
      return BadBlocksConfig.BadBlocksNumbersArraySizeSector$1;

   return BadBlocksConfig.BadBlocksNumbersArraySizeSector$0;
}
