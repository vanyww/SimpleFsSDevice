#pragma once

#include "SimpleFsSDevice/public.h"

typedef SimpleFsSDeviceSector Sector;

typedef struct
{
   const Sector *Sector;
   uintptr_t Cursor;
   bool IsInBounds;
} Stream;

typedef Stream WriteStream;
typedef Stream ReadStream;

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(SimpleFs);

SDEVICE_RUNTIME_DATA_DECLARATION(SimpleFs)
{
   WriteStream *ActiveWriteStream;
   WriteStream *InactiveWriteStream;

   WriteStream Sector$0WriteStream;
   WriteStream Sector$1WriteStream;
};

SDEVICE_HANDLE_DECLARATION(SimpleFs);
SDEVICE_INTERNAL_ALIASES_DECLARATION(SimpleFs);
