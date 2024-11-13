#pragma once

#include "SDeviceCore/common.h"

#define NTH_BIT(n) (1U << n)
#define SET_NTH_BIT(value, n) SET_BITS(value, NTH_BIT(n))
#define READ_NTH_BIT(value, n) READ_BITS(value, NTH_BIT(n))
#define CLEAR_NTH_BIT(value, n) CLEAR_BITS(value, NTH_BIT(n))
#define COUNT_SET_BITS(value) __builtin_popcount(value)
