#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t Crc8Type;

Crc8Type SimpleFsSDeviceComputeCrc8(const void *, size_t);
Crc8Type SimpleFsSDeviceUpdateCrc8(const void *, Crc8Type, size_t);
