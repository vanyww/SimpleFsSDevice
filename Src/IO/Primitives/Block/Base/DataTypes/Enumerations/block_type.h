#pragma once

#include "Common/double_bit_field.h"

#include <stdint.h>

#define IS_VALID_BLOCK_TYPE(type) (                                                                                    \
{                                                                                                                      \
   __auto_type _type = (type);                                                                                         \
                                                                                                                       \
   _type == BLOCK_TYPE_HEADER        ||                                                                                \
   _type == BLOCK_TYPE_FILE_AREA_TAG ||                                                                                \
   _type == BLOCK_TYPE_FILE_DATA     ||                                                                                \
   _type == BLOCK_TYPE_BAD_AREA_TAG;                                                                                   \
})

#define IS_VALID_SERVICE_BLOCK_TYPE(type) (                                                                            \
{                                                                                                                      \
   __auto_type _type = (type);                                                                                         \
                                                                                                                       \
   _type == BLOCK_TYPE_HEADER        ||                                                                                \
   _type == BLOCK_TYPE_FILE_AREA_TAG ||                                                                                \
   _type == BLOCK_TYPE_BAD_AREA_TAG;                                                                                   \
})

#define IS_VALID_AREA_TAG_BLOCK_TYPE(type) (                                                                           \
{                                                                                                                      \
   __auto_type _type = (type);                                                                                         \
                                                                                                                       \
   _type == BLOCK_TYPE_FILE_AREA_TAG ||                                                                                \
   _type == BLOCK_TYPE_BAD_AREA_TAG;                                                                                   \
})

typedef enum __attribute__((packed))
{
   BLOCK_TYPE_HEADER        = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 0)),
   BLOCK_TYPE_FILE_DATA     = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 1)),
   BLOCK_TYPE_BAD_AREA_TAG  = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 2)),
   BLOCK_TYPE_FILE_AREA_TAG = CREATE_DOUBLE_BIT_FIELD((uint8_t)(1 << 3)),

   BLOCK_TYPE_ANY_AREA_TAG  = BLOCK_TYPE_BAD_AREA_TAG | BLOCK_TYPE_FILE_AREA_TAG,
   BLOCK_TYPE_ANY_SERVICE   = BLOCK_TYPE_ANY_AREA_TAG | BLOCK_TYPE_HEADER,
   BLOCK_TYPE_ANY           = BLOCK_TYPE_ANY_SERVICE  | BLOCK_TYPE_FILE_DATA
} BlockType;
