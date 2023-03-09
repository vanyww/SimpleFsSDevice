#pragma once

#include "../../../private.h"
#include "DataTypes/stream_seek_origin.h"
#include "../Block/Base/DataTypes/block.h"

#include "SDeviceCore/errors.h"

#define BLOCK_TO_MEMORY_ADDRESS(value) ((value) * sizeof(Block))
#define MEMORY_TO_BLOCK_ADDRESS(value) ((value) / sizeof(Block))

static inline Stream CloneStream(Stream *stream)
{
   SDeviceDebugAssert(stream != NULL);

   return *stream;
}

static inline size_t GetStreamTotalLength(Stream *stream)
{
   SDeviceDebugAssert(stream != NULL);

   return GetSectorTotalLength(stream->Sector);
}

static inline size_t GetStreamToStartLength(Stream *stream)
{
   SDeviceDebugAssert(stream != NULL);
   SDeviceDebugAssert(stream->IsInBounds);

   return stream->Cursor + 1;
}

static inline size_t GetStreamToEndLength(Stream *stream)
{
   SDeviceDebugAssert(stream != NULL);
   SDeviceDebugAssert(stream->IsInBounds);

   return GetStreamTotalLength(stream) - stream->Cursor;
}

static inline void SeekStream(Stream *stream, SeekStreamOrigin origin, intptr_t offset)
{
   SDeviceDebugAssert(stream != NULL);
   SDeviceDebugAssert(IS_VALID_SEEK_STREAM_ORIGIN(origin));

   switch(origin)
   {
      case SEEK_STREAM_ORIGIN_CURRENT:
         if(stream->IsInBounds)
         {
            stream->Cursor += offset;
            stream->IsInBounds =
                  (offset < 0) ? GetStreamToStartLength(stream) > -offset : GetStreamToEndLength(stream) > offset;
         }
         return;

      case SEEK_STREAM_ORIGIN_BEGINNING:
         stream->Cursor = offset;
         break;

      case SEEK_STREAM_ORIGIN_ENDING:
         stream->Cursor = (GetStreamTotalLength(stream) - 1) - offset;
         break;
   }

   stream->IsInBounds = offset >= 0 && offset <= GetStreamTotalLength(stream) - 1;
}
