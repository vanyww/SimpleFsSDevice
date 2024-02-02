#pragma once

#include "common.h"
#include "../Sector/sector.h"

static inline void EraseStreamSector(ThisHandle *handle, Stream *stream)
{
   EraseSector(handle, stream->Sector);
}
