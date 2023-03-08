#pragma once

#include "../Base/area_tag_block.h"
#include "Common/size.h"
#include "Common/crc.h"

static inline FileAreaTagBlock BuildFileAreaTagBlock(ThisHandle *handle, uint16_t fileId, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(size <= MAX_FILE_SIZE);
   SDeviceDebugAssert(data != NULL || size == 0);

   FileAreaTagBlock block =
   {
      .Type = BLOCK_TYPE_FILE_AREA_TAG,
      .FileAreaLength = ComputeFileAreaLength(size),
      .LastFileBlockDataSize = ComputeFileLastBlockDataSize(size),
      .FileId = fileId,
      .FileCrc = ComputeFileDataCrc(handle, data, size)
   };
   block.BlockCrc = ComputeServiceBlockCrc(handle, block);

   return block;
}

static inline bool IsFileAreaTagBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_FILE_AREA_TAG);
}

static inline size_t ComputeFileAreaFileSize(FileAreaTagBlock block)
{
   SDeviceDebugAssert(IsFileAreaTagBlock(block));

   return ComputeFileSize(block.FileAreaLength, block.LastFileBlockDataSize);
}
