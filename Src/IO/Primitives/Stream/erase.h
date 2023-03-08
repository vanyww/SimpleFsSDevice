#pragma once

#include "common.h"
#include "../Sector/sector.h"

static inline void EraseStreamSector(ThisHandle *handle, Stream *stream)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   EraseSector(handle, stream->Sector);
}
