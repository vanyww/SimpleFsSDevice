#pragma once

#include "../../Base/Common/Crc/crc.h"

static inline uint16_t ComputeFileDataCrc(ThisHandle *handle, const void *data, size_t size)
{
   return ComputeCrc16(handle, data, size);
}

static inline uint16_t UpdateFileDataCrc(ThisHandle *handle, uint16_t crc, const void *data, size_t size)
{
   return UpdateCrc16(handle, crc, data, size);
}
