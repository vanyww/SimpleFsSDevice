#pragma once

#include "data_types.h"
#include "../../../IO/Selection/selection.h"
#include "../../../IO/Selection/Filters/ExceptBadAreas/filter.h"
#include "../../../IO/Primitives/Block/File/file_area_tag_block.h"
#include "../../../IO/Primitives/Block/File/file_data_block.h"

static inline FileAreaInfo BuildFileAreaInfo(FileAreaTagBlock tagBlock)
{
   SDeviceDebugAssert(IsFileAreaTagBlock(tagBlock));

   return (FileAreaInfo)
   {
      .FileId = tagBlock.FileId,
      .FileCrc = tagBlock.FileCrc,
      .FileSize = ComputeFileAreaFileSize(tagBlock)
   };
}

static inline FileAreaHandle CreateFileAreaHandle(FileAreaInfo *fileInfo, ReadStream *fileStream)
{
   SDeviceDebugAssert(fileInfo != NULL);
   SDeviceDebugAssert(fileStream != NULL);

   FileAreaHandle area = { .AreaInfo = fileInfo, .FileStream = CloneStream(fileStream) };
   SeekStream(&area.FileStream, SEEK_STREAM_ORIGIN_CURRENT, -1);
   return area;
}

static bool TryReadFileAreaData(ThisHandle *handle, FileAreaHandle *area, void *buffer)
{
   SDeviceDebugAssert(buffer != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(area != NULL);

   Block readBlock;
   const SelectionFilter filters[] = { COMPOSE_SELECTION_FILTER(ExceptBadAreas) };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));
   size_t leftToReadSize = area->AreaInfo->FileSize;

   while(TrySelectNextStreamBlock(handle, &area->FileStream, &selector, &readBlock))
   {
      if(!IsFileDataBlock(readBlock))
         break;

      FileDataBlock blockAsFileData = readBlock.AsFileData;

      if(leftToReadSize <= SIZEOF_MEMBER(FileDataBlock, Data))
      {
         memcpy(buffer, blockAsFileData.Data, leftToReadSize);
         return ComputeFileDataCrc(handle, buffer, area->AreaInfo->FileSize) == area->AreaInfo->FileCrc;
      }

      memcpy(buffer, blockAsFileData.Data, sizeof(blockAsFileData.Data));
      buffer += sizeof(blockAsFileData.Data);
      leftToReadSize -= sizeof(blockAsFileData.Data);
   }

   return false;
}
