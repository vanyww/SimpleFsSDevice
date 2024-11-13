#pragma once

#include "Common/double_bit_field.h"

#include <stdint.h>

#define IS_VALID_SECTOR_STATE(state) (                                                                                 \
{                                                                                                                      \
   __auto_type _mState = (state);                                                                                      \
                                                                                                                       \
   _mState == SECTOR_STATE_ACTIVE           ||                                                                         \
   _mState == SECTOR_STATE_TRANSFER_ONGOING ||                                                                         \
   _mState == SECTOR_STATE_TRANSFER_END     ||                                                                         \
   _mState == SECTOR_STATE_ERASED;                                                                                     \
})

typedef enum __attribute__((packed))
{
   SECTOR_STATE_ACTIVE           = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 0)),
   SECTOR_STATE_TRANSFER_ONGOING = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 1)),
   SECTOR_STATE_TRANSFER_END     = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 2)),
   SECTOR_STATE_ERASED           = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 3))
} SectorState;
