#pragma once

#include "../../IO/Primitives/Block/File/file_area_tag_block.h"
#include "../../IO/Primitives/Block/File/file_data_block.h"
#include "../../IO/Primitives/Stream/write.h"

static bool TryWriteStreamFile(ThisHandle *handle, WriteStream *stream, uint16_t fileIdx, const void *data, size_t size)
{
   if(!stream->IsInBounds || GetStreamToEndLength(stream) < ComputeTotalFileLength(size))
      return false;

   if(size > 0)
   {
      size_t leftSize = size;
      size_t blockDataSize = ComputeFileLastBlockDataSize(leftSize);
      data += leftSize;

      do
      {
         data -= blockDataSize;

         FileDataBlock fileDataBlock = BuildFileDataBlock(handle, data, blockDataSize);
         if(!TryWriteStreamGoodBlock(handle, stream, fileDataBlock))
            return false;

         leftSize -= blockDataSize;
         blockDataSize = SIZEOF_MEMBER(FileDataBlock, Data);
      } while(leftSize > 0);
   }

   FileAreaTagBlock fileAreaTag = BuildFileAreaTagBlock(handle, fileIdx, data, size);
   return TryWriteStreamGoodBlock(handle, stream, fileAreaTag);
}
