#pragma once

#include "transfer.h"

static size_t ComputeEmptyStreamBadBlocksCount(ThisHandle *handle, WriteStream *stream)
{
   /* "empty" stream sector must have one header block written, which can also cause single bad block tag to appear */
   size_t writtenBlocksCount = GetStreamToStartLength(stream) - 1;
   size_t badBlocksCount = (writtenBlocksCount <= 1) ? 0 : writtenBlocksCount - 2;

   while(stream->IsInBounds)
   {
      if(!TryWriteStreamBlock(handle, stream, GetFilledBlock(handle)))
         badBlocksCount++;
   }

   FormatStreamSector(handle, stream);

   return badBlocksCount;
}

static size_t ComputeTotalBadBlocksCount(ThisHandle *handle)
{
   size_t totalBadBlocksCount = 0;

   totalBadBlocksCount += ComputeEmptyStreamBadBlocksCount(handle, GetInactiveWriteStream(handle));
   TransferActiveStream(handle, NULL);
   totalBadBlocksCount += ComputeEmptyStreamBadBlocksCount(handle, GetInactiveWriteStream(handle));

   return totalBadBlocksCount;
}
