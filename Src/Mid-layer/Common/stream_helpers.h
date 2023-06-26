#pragma once

#include "../../IO/Primitives/Stream/common.h"

static inline void SetActiveWriteStream(ThisHandle *handle, WriteStream *stream)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   handle->Runtime.ActiveWriteStream = stream;
}

static inline WriteStream * GetActiveWriteStream(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return handle->Runtime.ActiveWriteStream;
}

static inline void SetInactiveWriteStream(ThisHandle *handle, WriteStream *stream)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   handle->Runtime.InactiveWriteStream = stream;
}

static inline WriteStream * GetInactiveWriteStream(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return handle->Runtime.InactiveWriteStream;
}

static inline void SwitchWriteStreams(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   WriteStream *activeStream = GetActiveWriteStream(handle);
   WriteStream *inactiveStream = GetInactiveWriteStream(handle);

   SetActiveWriteStream(handle, inactiveStream);
   SetInactiveWriteStream(handle, activeStream);
}

static inline ReadStream BuildReadStream(WriteStream *writeStream)
{
   SDeviceDebugAssert(writeStream != NULL);

   ReadStream readStream = CloneStream(writeStream);

   if(readStream.IsInBounds)
      SeekStream(&readStream, SEEK_STREAM_ORIGIN_CURRENT, -1);
   else
      SeekStream(&readStream, SEEK_STREAM_ORIGIN_ENDING, 0);

   return readStream;
}

static inline ReadStream BuildActiveReadStream(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return BuildReadStream(GetActiveWriteStream(handle));
}

static inline ReadStream BuildInactiveReadStream(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return BuildReadStream(GetInactiveWriteStream(handle));
}
