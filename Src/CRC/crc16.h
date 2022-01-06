#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t CrcType;

CrcType ComputeCrc16(const void *, size_t);
CrcType UpdateCrc16(const void *, size_t, uint16_t);
