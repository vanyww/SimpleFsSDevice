#pragma once

#include "Common/memory_limits.h"
#include "DataTypes/block.h"

static inline Block GetEmptyBlock(ThisHandle *handle)
{
   return (Block){ .AsValue = GetEmptyUInt64MemoryValue(handle) };
}

static inline Block GetFilledBlock(ThisHandle *handle)
{
   return (Block){ .AsValue = GetFilledUInt64MemoryValue(handle) };
}

static inline bool AreBlocksEqual(Block block$0, Block block$1)
{
   return block$0.AsValue == block$1.AsValue;
}

static inline bool IsBlockEmpty(ThisHandle *handle, Block block)
{
   return AreBlocksEqual(GetEmptyBlock(handle), block);
}

static inline bool HasBlockValidType(Block block)
{
   return IS_VALID_BLOCK_TYPE(block.Type);
}

static inline bool IsBlockOfType(Block block, BlockType type)
{
   return READ_BITS(block.Type, type) != 0;
}
