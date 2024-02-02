#pragma once

#include "../Block/Base/common_block.h"
#include "../Sector/sector.h"
#include "common.h"

static inline void PeekStreamBlock(ThisHandle *handle, ReadStream *stream, Block *block)
{
   ReadSectorUInt64(handle, stream->Sector, BLOCK_TO_MEMORY_ADDRESS(stream->Cursor), &block->AsValue);
}

static inline void ReadStreamBlock(ThisHandle *handle, ReadStream *stream, Block *block)
{
   PeekStreamBlock(handle, stream, block);
   SeekStream(stream, SEEK_STREAM_ORIGIN_CURRENT, -1);
}
