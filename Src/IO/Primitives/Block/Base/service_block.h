#pragma once

#include "Common/Crc/crc.h"
#include "common_block.h"

static inline bool IsServiceBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_ANY_SERVICE);
}

static inline uint8_t ComputeServiceBlockCrc(ThisHandle *handle, ServiceBlock block)
{
   return ComputeCrc8(handle, block.BlockData, sizeof(block.BlockData));
}

static inline bool HasServiceBlockValidCrc(ThisHandle *handle, ServiceBlock block)
{
   return block.BlockCrc == ComputeServiceBlockCrc(handle, block);
}
