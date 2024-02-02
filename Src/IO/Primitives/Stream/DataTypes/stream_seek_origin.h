#pragma once

#define IS_VALID_SEEK_STREAM_ORIGIN(value) (                                                                           \
{                                                                                                                      \
   __auto_type _value = (value);                                                                                       \
   _value == SEEK_STREAM_ORIGIN_BEGINNING ||                                                                           \
   _value == SEEK_STREAM_ORIGIN_ENDING    ||                                                                           \
   _value == SEEK_STREAM_ORIGIN_CURRENT;                                                                               \
})

typedef enum
{
   SEEK_STREAM_ORIGIN_BEGINNING,
   SEEK_STREAM_ORIGIN_ENDING,
   SEEK_STREAM_ORIGIN_CURRENT
} SeekStreamOrigin;
