#pragma once

#include "data_types.h"
#include "../../../IO/Selection/selection.h"
#include "../../../IO/Selection/Filters/ExceptBadAreas/filter.h"
#include "../../../IO/Primitives/Block/File/file_area_tag_block.h"
#include "../../../IO/Primitives/Block/File/file_data_block.h"

static inline FileAreaInfo BuildFileAreaInfo(FileAreaTagBlock tagBlock)
{
   return (FileAreaInfo)
   {
      .FileIdx  = tagBlock.FileIdx,
      .FileCrc  = tagBlock.FileCrc,
      .FileSize = ComputeFileAreaFileSize(tagBlock)
   };
}

static inline FileAreaHandle CreateFileAreaHandle(FileAreaInfo *fileInfo, ReadStream *fileStream)
{
   FileAreaHandle area = { .AreaInfo = fileInfo, .FileStream = CloneStream(fileStream) };
   SeekStream(&area.FileStream, SEEK_STREAM_ORIGIN_CURRENT, -1);
   return area;
}

static bool TryReadFileAreaData(ThisHandle *handle, FileAreaHandle *area, void *buffer)
{
   Block readBlock;
   const SelectionFilter filters[] = { COMPOSE_SELECTION_FILTER(ExceptBadAreas) };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));
   size_t leftToReadSize = area->AreaInfo->FileSize;
   void *writeBuffer = buffer;

   while(TrySelectNextStreamBlock(handle, &area->FileStream, &selector, &readBlock))
   {
      if(!IsFileDataBlock(readBlock))
         break;

      FileDataBlock blockAsFileData = readBlock.AsFileData;

      if(leftToReadSize <= SIZEOF_MEMBER(FileDataBlock, Data))
      {
         memcpy(writeBuffer, blockAsFileData.Data, leftToReadSize);

         return ComputeFileDataCrc(handle, buffer, area->AreaInfo->FileSize) == area->AreaInfo->FileCrc;
      }

      memcpy(writeBuffer, blockAsFileData.Data, sizeof(blockAsFileData.Data));
      writeBuffer += sizeof(blockAsFileData.Data);
      leftToReadSize -= sizeof(blockAsFileData.Data);
   }

   return false;
}
