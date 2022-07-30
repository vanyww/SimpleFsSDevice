#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t CrcType;

CrcType SimpleFsSDeviceComputeCrc(const void *, size_t);
CrcType SimpleFsSDeviceUpdateCrc(const void *, CrcType, size_t);
