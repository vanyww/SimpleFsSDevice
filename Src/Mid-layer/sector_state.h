#pragma once

#include "../IO/Selection/selection.h"
#include "../IO/Selection/Filters/OnlyValidHeader/filter.h"
#include "../IO/Selection/Filters/ValidServiceOfType/filter.h"
#include "../IO/Primitives/Block/header_block.h"
#include "../IO/Primitives/Stream/write.h"

static bool TryReadStreamSectorState(ThisHandle *handle, ReadStream *stream, SectorState *state)
{
   SDeviceDebugAssert(state != NULL);
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   Block readBlock;
   const SelectionFilter filters[] =
   {
      COMPOSE_SELECTION_FILTER(ValidServiceOfType, BLOCK_TYPE_HEADER),
      COMPOSE_SELECTION_FILTER(OnlyValidHeader)
   };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));

   if(!TrySelectNextStreamBlock(handle, stream, &selector, &readBlock))
      return false;

   HeaderBlock *blockAsHeader = &readBlock.AsHeader;
   *state = blockAsHeader->SectorState;

   return true;
}

static inline void WriteStreamSectorState(ThisHandle *handle, WriteStream *stream, SectorState state)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   if(!TryWriteStreamGoodBlock(handle, stream, BuildHeaderBlock(handle, state)))
      SDevicePanic(handle, SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY);
}
