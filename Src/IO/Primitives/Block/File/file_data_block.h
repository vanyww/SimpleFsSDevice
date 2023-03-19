#pragma once

#include "../Base/common_block.h"

#include <memory.h>

static inline FileDataBlock BuildFileDataBlock(ThisHandle *handle, const void *data, size_t size)
{
   SDeviceDebugAssert(data != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(size > 0 && size <= SIZEOF_MEMBER(FileDataBlock, Data));

   FileDataBlock block =
   {
      .Type = BLOCK_TYPE_FILE_DATA,
      .Data = { [0 ... SIZEOF_MEMBER(FileDataBlock, Data) - 1] = GetEmptyUInt8MemoryValue(handle) }
   };
   memcpy(block.Data, data, size);

   return block;
}

static inline bool IsFileDataBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_FILE_DATA);
}
