#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t CrcType;

CrcType Crc16Compute(const void *, size_t);
CrcType Crc16Update(const void *, size_t, uint16_t);
