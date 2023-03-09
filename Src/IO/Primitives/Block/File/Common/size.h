#pragma once

#include "../../Base/DataTypes/block.h"

#define MAX_FILE_AREA_LENGTH UINT8_MAX
#define MAX_FILE_SIZE ((MAX_FILE_AREA_LENGTH + 1) * SIZEOF_MEMBER(FileDataBlock, Data))

static inline size_t ComputeFileAreaLength(size_t fileSize)
{
   return fileSize / SIZEOF_MEMBER(FileDataBlock, Data);
}

static inline size_t ComputeTotalFileLength(size_t fileSize)
{
   return ComputeFileAreaLength(fileSize) + 1;
}

static inline size_t ComputeFileLastBlockDataSize(uint16_t fileSize)
{
   if(fileSize == 0)
      return 0;

   size_t fileSizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);
   return (fileSizeRemainder != 0) ? fileSizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);
}

static inline size_t ComputeFileSize(size_t fileAreaLength, size_t fileLastBlockDataSize)
{
   return fileAreaLength * SIZEOF_MEMBER(FileDataBlock, Data) + fileLastBlockDataSize;
}
