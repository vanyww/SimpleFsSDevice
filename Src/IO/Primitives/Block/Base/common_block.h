#pragma once

#include "Common/memory_limits.h"
#include "DataTypes/block.h"

static inline Block GetEmptyBlock(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return (Block){ .AsValue = GetEmptyUInt64MemoryValue(handle) };
}

static inline Block GetFilledBlock(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return (Block){ .AsValue = GetFilledUInt64MemoryValue(handle) };
}

static inline bool AreBlocksEqual(Block block$0, Block block$1)
{
   return block$0.AsValue == block$1.AsValue;
}

static inline bool IsBlockEmpty(ThisHandle *handle, Block block)
{
   SDeviceDebugAssert(handle != NULL);

   return AreBlocksEqual(GetEmptyBlock(handle), block);
}

static inline bool HasBlockValidType(Block block)
{
   return IS_VALID_BLOCK_TYPE(block.Type);
}

static inline bool IsBlockOfType(Block block, BlockType type)
{
   SDeviceDebugAssert(IS_VALID_DOUBLE_BIT_FIELD(type));
   SDeviceDebugAssert(HasBlockValidType(block));

   return READ_BITS(block.Type, type) != 0;
}
