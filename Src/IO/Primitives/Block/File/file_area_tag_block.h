#pragma once

#include "../Base/area_tag_block.h"
#include "Common/size.h"
#include "Common/crc.h"

static inline FileAreaTagBlock BuildFileAreaTagBlock(
      ThisHandle *handle,
      uint16_t    fileIdx,
      const void *data,
      size_t      size)
{
   FileAreaTagBlock block =
   {
      .Type                  = BLOCK_TYPE_FILE_AREA_TAG,
      .FileAreaLength        = ComputeFileAreaLength(size),
      .LastFileBlockDataSize = ComputeFileLastBlockDataSize(size),
      .FileIdx               = fileIdx,
      .FileCrc               = ComputeFileDataCrc(handle, data, size)
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
   return ComputeFileSize(block.FileAreaLength, block.LastFileBlockDataSize);
}
