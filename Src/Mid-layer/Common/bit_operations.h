#pragma once

#include "SDeviceCore/common.h"

#include <stdint.h>
#include <stddef.h>

#define NTH_BIT(n) (1U << n)
#define SET_NTH_BIT(value, n) SET_BITS(value, NTH_BIT(n))
#define READ_NTH_BIT(value, n) READ_BITS(value, NTH_BIT(n))
#define CLEAR_NTH_BIT(value, n) CLEAR_BITS(value, NTH_BIT(n))

size_t CountSetBits(uint32_t value)
{
   value = value - ((value >> 1) & 0x55555555);
   value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
   value = (value + (value >> 4)) & 0x0F0F0F0F;
   return (value * 0x01010101) >> 24;
}
