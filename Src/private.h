#pragma once

#include "SimpleFsSDevice/public.h"

#define IS_VALID_THIS_HANDLE(handle) SDEVICE_IS_VALID_HANDLE(SimpleFs, handle)

typedef SimpleFsSDeviceSector ThisSector;

typedef struct
{
   const ThisSector *Sector;
   uintptr_t         Cursor;
   bool              IsInBounds;
} Stream;

typedef Stream WriteStream;
typedef Stream ReadStream;

SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(SimpleFs);

SDEVICE_RUNTIME_DATA_DECLARATION(SimpleFs)
{
   WriteStream *ActiveWriteStream;
   WriteStream *InactiveWriteStream;

   WriteStream  MainSectorWriteStream;
   WriteStream  AuxiliarySectorWriteStream;
};

SDEVICE_HANDLE_DECLARATION(SimpleFs);
SDEVICE_INTERNAL_ALIASES_DECLARATION(SimpleFs);
