#pragma once

#include "../../../../../private.h"

#include "SDeviceCore/errors.h"

static inline uint8_t GetEmptyUInt8MemoryValue(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return handle->Init.IsMemoryErasingToZero ? 0 : UINT8_MAX;
}

static inline uint64_t GetEmptyUInt64MemoryValue(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return handle->Init.IsMemoryErasingToZero ? 0ULL : UINT64_MAX;
}

static inline uint8_t GetFilledUInt8MemoryValue(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return ~GetEmptyUInt8MemoryValue(handle);
}

static inline uint64_t GetFilledUInt64MemoryValue(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   return ~GetEmptyUInt64MemoryValue(handle);
}
