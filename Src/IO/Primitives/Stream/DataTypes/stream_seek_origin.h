#pragma once

#define IS_VALID_SEEK_STREAM_ORIGIN(value) (                                                                           \
{                                                                                                                      \
   __auto_type _mValue = (value);                                                                                      \
                                                                                                                       \
   _mValue == SEEK_STREAM_ORIGIN_BEGINNING ||                                                                          \
   _mValue == SEEK_STREAM_ORIGIN_ENDING    ||                                                                          \
   _mValue == SEEK_STREAM_ORIGIN_CURRENT;                                                                              \
})

typedef enum
{
   SEEK_STREAM_ORIGIN_BEGINNING,
   SEEK_STREAM_ORIGIN_ENDING,
   SEEK_STREAM_ORIGIN_CURRENT
} SeekStreamOrigin;
