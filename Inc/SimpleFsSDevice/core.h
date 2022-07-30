#pragma once

#include "SDeviceCore/core.h"

#include <stdbool.h>

#define __SIMPLE_FS_SDEVICE_SECTORS_COUNT 2

typedef uint64_t SimpleFsSDeviceBlockValue;
typedef uint16_t SimpleFsSDeviceAddress;

typedef struct
{
   void *Context;
   uintptr_t Address;
   size_t Size;
} SimpleFsSDeviceSector;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(SimpleFs);
__SDEVICE_INIT_DATA_FORWARD_DECLARATION(SimpleFs);

typedef enum
{
   SIMPLE_FS_SDEVICE_EXCEPTION_READ_FAIL        = 0x01,
   SIMPLE_FS_SDEVICE_EXCEPTION_WRITE_FAIL       = 0x02,
   SIMPLE_FS_SDEVICE_EXCEPTION_ERASE_FAIL       = 0x03,
   SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY    = 0x04
} SimpleFsSDeviceException;

typedef enum
{
   SIMPLE_FS_SDEVICE_STATUS_OK                  = 0x00,
   SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_HEADER    = 0x01,
   SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_FILE      = 0x02
} SimpleFsSDeviceStatus;

struct __SDEVICE_INIT_DATA(SimpleFs)
{
   bool (* TryReadBlock)(__SDEVICE_HANDLE(SimpleFs) *, uintptr_t, SimpleFsSDeviceBlockValue *);
   bool (* TryWriteBlock)(__SDEVICE_HANDLE(SimpleFs) *, uintptr_t, const SimpleFsSDeviceBlockValue *);
   bool (* TryEraseSector)(__SDEVICE_HANDLE(SimpleFs) *, const SimpleFsSDeviceSector *);
   SimpleFsSDeviceSector Sectors[__SIMPLE_FS_SDEVICE_SECTORS_COUNT];
   SimpleFsSDeviceAddress MaxUsedAddress;
   bool IsErasingToZero;
};

__SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs,,,);
__SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFs,);

/* Satty's interface end */

bool SimpleFsSDeviceTryGetFileSize(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress, size_t *);
bool SimpleFsSDeviceTryRead(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress, size_t, void *);
bool SimpleFsSDeviceTryDelete(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress);
bool SimpleFsSDeviceTryWrite(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress, size_t, const void *);
