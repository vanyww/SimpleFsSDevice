#pragma once

#include "../Block/bad_area_tag_block.h"
#include "../Sector/sector.h"
#include "common.h"

#include "SDeviceCore/errors.h"

static inline bool TryPutStreamBlock(ThisHandle *handle, WriteStream *stream, Block block)
{
   WriteSectorUInt64(handle, stream->Sector, BLOCK_TO_MEMORY_ADDRESS(stream->Cursor), block.AsValue);

   Block readBlock;
   ReadSectorUInt64(handle, stream->Sector, BLOCK_TO_MEMORY_ADDRESS(stream->Cursor), &readBlock.AsValue);

   return AreBlocksEqual(readBlock, block);
}

static inline bool TryWriteStreamBlock(ThisHandle *handle, WriteStream *stream, Block block)
{
   bool status = TryPutStreamBlock(handle, stream, block);
   SeekStream(stream, SEEK_STREAM_ORIGIN_CURRENT, +1);

   return status;
}

static bool TryWriteStreamGoodBlock(ThisHandle *handle, WriteStream *stream, Block block)
{
   uint8_t badAreaBadBlocksCount = 0;

   while(stream->IsInBounds)
   {
      if(!badAreaBadBlocksCount)
      {
         if(TryWriteStreamBlock(handle, stream, block))
            return true;

         badAreaBadBlocksCount = 1;
         continue;
      }

      if(TryWriteStreamBlock(handle, stream, BuildBadAreaTagBlock(handle, badAreaBadBlocksCount)))
      {
         badAreaBadBlocksCount = 0;
      }
      else if(++badAreaBadBlocksCount == MAX_BAD_AREA_LENGTH)
      {
         SDeviceThrowPanic(handle, SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW);
      }
   }

   return false;
}
