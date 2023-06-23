#pragma once

#include "Base/area_tag_block.h"

#define MAX_BAD_AREA_LENGTH UINT8_MAX

static inline BadAreaTagBlock BuildBadAreaTagBlock(ThisHandle *handle, uint16_t badBlocksCount)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(badBlocksCount > 0);

   BadAreaTagBlock block =
   {
      .Type = BLOCK_TYPE_BAD_AREA_TAG,
      .BadAreaLength = badBlocksCount,
      .Padding = { [0 ... SIZEOF_MEMBER(BadAreaTagBlock, Padding) - 1] = GetEmptyUInt8MemoryValue(handle) }
   };
   block.BlockCrc = ComputeServiceBlockCrc(handle, block);

   return block;
}

static inline bool IsBadAreaTagBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_BAD_AREA_TAG);
}
