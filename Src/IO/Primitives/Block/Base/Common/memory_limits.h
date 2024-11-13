#pragma once

#include "../../../../../private.h"

static inline uint8_t GetEmptyUInt8MemoryValue(ThisHandle *handle)
{
   return (handle->Init->IsMemoryErasingToZero) ? 0 : UINT8_MAX;
}

static inline uint64_t GetEmptyUInt64MemoryValue(ThisHandle *handle)
{
   return (handle->Init->IsMemoryErasingToZero) ? 0ULL : UINT64_MAX;
}

static inline uint8_t GetFilledUInt8MemoryValue(ThisHandle *handle)
{
   return ~GetEmptyUInt8MemoryValue(handle);
}

static inline uint64_t GetFilledUInt64MemoryValue(ThisHandle *handle)
{
   return ~GetEmptyUInt64MemoryValue(handle);
}
