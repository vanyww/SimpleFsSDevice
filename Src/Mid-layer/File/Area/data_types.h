#pragma once

#include "../../../private.h"

typedef struct
{
   uint16_t FileIdx;
   uint16_t FileCrc;
   size_t   FileSize;
} FileAreaInfo;

typedef struct
{
   FileAreaInfo *AreaInfo;
   ReadStream    FileStream;
} FileAreaHandle;
