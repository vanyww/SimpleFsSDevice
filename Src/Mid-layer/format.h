#pragma once

#include "../IO/Primitives/Stream/erase.h"
#include "Common/stream_helpers.h"
#include "sector_state.h"

static void FormatStreamSector(ThisHandle *handle, Stream *stream)
{
   EraseStreamSector(handle, stream);
   SeekStream(stream, SEEK_STREAM_ORIGIN_BEGINNING, 0);
   WriteStreamSectorState(handle, stream, SECTOR_STATE_ERASED);
}

static void FormatMemory(ThisHandle *handle)
{
   FormatStreamSector(handle, GetActiveWriteStream(handle));
   WriteStreamSectorState(handle, GetInactiveWriteStream(handle), SECTOR_STATE_ACTIVE);
   SwitchWriteStreams(handle);
}

static void ForceReformatMemory(ThisHandle *handle)
{
   FormatStreamSector(handle, &handle->Runtime->MainSectorWriteStream);
   FormatStreamSector(handle, &handle->Runtime->AuxiliarySectorWriteStream);

   WriteStreamSectorState(handle, &handle->Runtime->MainSectorWriteStream, SECTOR_STATE_ACTIVE);

   SetActiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
   SetInactiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
}
