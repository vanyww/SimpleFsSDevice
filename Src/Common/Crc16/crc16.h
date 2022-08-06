#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t Crc16Type;

Crc16Type SimpleFsSDeviceComputeCrc16(const void *, size_t);
Crc16Type SimpleFsSDeviceUpdateCrc16(const void *, Crc16Type, size_t);
