#pragma once

#include "../Primitives/Stream/read.h"
#include "Filters/selection_filter_interface.h"

typedef struct
{
   intptr_t NextBlockOffset;
} BlockSelectorContext;

typedef struct
{
   const SelectionFilter *Filters;
   size_t FiltersCount;
   BlockSelectorContext Context;
} BlockSelector;

static BlockSelector CreateBlockSelector(ThisHandle *handle, const SelectionFilter *filters, size_t filtersCount)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(filters != NULL || filtersCount == 0);

   for(size_t i = 0; i < filtersCount; i++)
   {
      const SelectionFilter *filter = &filters[i];

      SDeviceDebugAssert(filter->Interface != NULL);

      if(filter->Interface->ContextInitFunction != NULL)
         filter->Interface->ContextInitFunction(handle, filter->Parameters, filter->Context);
   }

   return (BlockSelector)
   {
      .Filters = filters,
      .FiltersCount = filtersCount,
      .Context = { .NextBlockOffset = 0 }
   };
}

static bool TrySelectNextStreamBlock(ThisHandle *handle, Stream *stream, BlockSelector *selector, Block *block)
{
   SDeviceDebugAssert(block != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);
   SDeviceDebugAssert(selector != NULL);

   SeekStream(stream, SEEK_STREAM_ORIGIN_CURRENT, selector->Context.NextBlockOffset);

   while(stream->IsInBounds)
   {
      Block readBlock;
      intptr_t nextBlockOffset = -1;
      bool blockFound = true;

      PeekStreamBlock(handle, stream, &readBlock);

      for(size_t i = 0; i < selector->FiltersCount; i++)
      {
         const SelectionFilter *filter = &selector->Filters[i];

         SDeviceDebugAssert(filter->Interface != NULL);
         SDeviceDebugAssert(filter->Interface->FilterFunction != NULL);

         SELECTION_FILTER_FUNCTION_POINTER(filterFunction) = filter->Interface->FilterFunction;
         FilteringResult filteringResult = filterFunction(handle, filter->Parameters, filter->Context, readBlock);

         if(filteringResult.NextBlockOffset < nextBlockOffset)
            nextBlockOffset = filteringResult.NextBlockOffset;

         if(!filteringResult.IsBlockSelected)
         {
            SeekStream(stream, SEEK_STREAM_ORIGIN_CURRENT, nextBlockOffset);
            blockFound = false;
            break;
         }
      }

      if(blockFound)
      {
         selector->Context.NextBlockOffset = nextBlockOffset;
         *block = readBlock;
         return true;
      }
   }

   return false;
}