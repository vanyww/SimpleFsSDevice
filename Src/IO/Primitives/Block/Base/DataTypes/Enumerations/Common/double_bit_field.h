#pragma once

#include "SDeviceCore/common.h"

#define GET_VALUE_LOW_HALF_MASK(value) ((1 << BIT_SIZEOF(value) / 2) - 1)
#define GET_VALUE_HIGH_HALF_MASK(value) (~GET_LOW_VALUE_HALF_MASK(value))

#define GET_VALUE_HIGH_HALF(value) ((value) >> BIT_SIZEOF(value) / 2)
#define GET_VALUE_LOW_HALF(value) ((value) & GET_VALUE_LOW_HALF_MASK(value))

#define CREATE_DOUBLE_BIT_FIELD(value)                                                                                 \
   (GET_VALUE_LOW_HALF(value) << BIT_SIZEOF(value) / 2 | GET_VALUE_LOW_HALF(value))

#define IS_VALID_DOUBLE_BIT_FIELD(value) (                                                                             \
{                                                                                                                      \
   __auto_type _value = (value);                                                                                       \
   GET_VALUE_HIGH_HALF(_value) == GET_VALUE_LOW_HALF(value);                                                           \
})
