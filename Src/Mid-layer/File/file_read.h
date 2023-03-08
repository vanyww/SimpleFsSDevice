#pragma once

#include "Area/file_area.h"
#include "../../IO/Selection/Filters/ValidServiceOfType/filter.h"
#include "../../IO/Selection/Filters/FileAreaTagWithFileId/filter.h"

typedef struct
{
   uint16_t Lowest;
   uint16_t Highest;
} FileIdsRange;

static bool TryReadStreamFileIdsRange(ThisHandle *handle, ReadStream *stream, FileIdsRange *range)
{
   SDeviceDebugAssert(range != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   Block readBlock;
   FileIdsRange idsRange = { .Lowest = 1, .Highest = 0 };
   const SelectionFilter filters[] = { COMPOSE_SELECTION_FILTER(ValidServiceOfType, BLOCK_TYPE_FILE_AREA_TAG) };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));

   while(TrySelectNextStreamBlock(handle, stream, &selector, &readBlock))
   {
      FileAreaTagBlock blockAsfileAreaTag = readBlock.AsFileAreaTag;
      uint16_t fileId = blockAsfileAreaTag.FileId;

      if(idsRange.Lowest > fileId)
         idsRange.Lowest = fileId;

      if(idsRange.Highest < fileId)
         idsRange.Highest = fileId;
   }

   /* initially (lowest > highest) (1 > 0), if at least one file area tag found, (lowest <= highest) always */
   if(idsRange.Lowest <= idsRange.Highest)
   {
      *range = idsRange;
      return true;
   }

   return false;
}

static size_t ReadStreamMaxFileSize(ThisHandle *handle, ReadStream *stream, uint16_t fileId)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   Block readBlock;
   size_t maxFileSize = 0;
   const SelectionFilter filters[] =
   {
      COMPOSE_SELECTION_FILTER(ValidServiceOfType, BLOCK_TYPE_FILE_AREA_TAG),
      COMPOSE_SELECTION_FILTER(FileAreaTagWithFileId, fileId),
   };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));

   while(TrySelectNextStreamBlock(handle, stream, &selector, &readBlock))
   {
      size_t currentFileSize = ComputeFileAreaFileSize(readBlock.AsFileAreaTag);

      if(currentFileSize == 0)
         break;

      if(currentFileSize > maxFileSize)
         maxFileSize = currentFileSize;
   }

   return maxFileSize;
}

static size_t ReadStreamFile(ThisHandle *handle, ReadStream *stream, uint16_t fileId, void *buffer, size_t maxFileSize)
{
   SDeviceDebugAssert(buffer != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);
   SDeviceDebugAssert(maxFileSize > 0);

   Block readBlock;
   const SelectionFilter filters[] =
   {
      COMPOSE_SELECTION_FILTER(ValidServiceOfType, BLOCK_TYPE_FILE_AREA_TAG),
      COMPOSE_SELECTION_FILTER(FileAreaTagWithFileId, fileId),
   };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));

   while(TrySelectNextStreamBlock(handle, stream, &selector, &readBlock))
   {
      FileAreaInfo fileInfo = BuildFileAreaInfo(readBlock.AsFileAreaTag);

      if(fileInfo.FileSize == 0 || fileInfo.FileSize > maxFileSize)
         break;

      FileAreaHandle areaHandle = CreateFileAreaHandle(&fileInfo, stream);

      if(TryReadFileAreaData(handle, &areaHandle, buffer))
         return fileInfo.FileSize;
   }

   return 0;
}
