#pragma once

#include "SimpleFsSDevice/core.h"
#include "CRC/crc.h"

__SDEVICE_RUNTIME_DATA_FORWARD_DECLARATION(SimpleFs);

typedef struct
{
   SimpleFsSDeviceSector *Sector;
   uintptr_t WriteCursor;
   uintptr_t ReadCursor;
} Iterator;

typedef struct
{
   uintptr_t MemoryAddress;
   SimpleFsSDeviceAddress Address;
   CrcType Crc;
   uint8_t Size;
   bool IsDeleted;
   bool IsValid;
} FileData;

struct __SDEVICE_RUNTIME_DATA(SimpleFs)
{
   Iterator *ActiveIterator;
   Iterator Iterators[__SIMPLE_FS_SDEVICE_SECTORS_COUNT];
   FileData FileDataCache;
};

__SDEVICE_HANDLE_DEFINITION(SimpleFs);
