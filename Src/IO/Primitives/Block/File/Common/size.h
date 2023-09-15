#pragma once

#include "../../Base/DataTypes/block.h"

#define MAX_FILE_AREA_LENGTH UINT8_MAX
#define MAX_FILE_SIZE ((MAX_FILE_AREA_LENGTH + 1) * SIZEOF_MEMBER(FileDataBlock, Data))

static inline uint8_t ComputeFileAreaLength(size_t fileSize)
{
   return CEIL_UINT_DIV(fileSize, SIZEOF_MEMBER(FileDataBlock, Data));
}

static inline size_t ComputeTotalFileLength(size_t fileSize)
{
   return ComputeFileAreaLength(fileSize) + 1;
}

static inline uint8_t ComputeFileLastBlockDataSize(size_t fileSize)
{
   if(fileSize == 0)
      return 0;

   uint8_t fileSizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);
   return (fileSizeRemainder != 0) ? fileSizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);
}

static inline size_t ComputeFileSize(uint8_t fileAreaLength, uint8_t fileLastBlockDataSize)
{
   SDeviceAssert(fileAreaLength >= 0);

   if (fileAreaLength > 0)
      return (fileAreaLength - 1) * SIZEOF_MEMBER(FileDataBlock, Data) + fileLastBlockDataSize;

   return 0;
}
