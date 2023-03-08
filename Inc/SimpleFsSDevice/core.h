#pragma once

#include "SDeviceCore/core.h"

#include <stdbool.h>

#if (__SDEVICE_CORE_VERSION_MAJOR != 2) || (__SDEVICE_CORE_VERSION_MINOR < 2)
#error SDevice core version is incorrect.
#endif

#define __SIMPLE_FS_SDEVICE_VERSION_MAJOR 2
#define __SIMPLE_FS_SDEVICE_VERSION_MINOR 0
#define __SIMPLE_FS_SDEVICE_VERSION_PATCH 0

#define __SIMPLE_FS_SDEVICE_CORE_VERSION ((SDeviceVersion)                                                             \
{                                                                                                                      \
   .Major = __SIMPLE_FS_SDEVICE_VERSION_MAJOR,                                                                         \
   .Minor = __SIMPLE_FS_SDEVICE_VERSION_MINOR,                                                                         \
   .Patch = __SIMPLE_FS_SDEVICE_VERSION_PATCH                                                                          \
})

typedef struct
{
   void *Context;
   size_t Size;
} SimpleFsSDeviceSector;

SDEVICE_HANDLE_FORWARD_DECLARATION(SimpleFs);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(SimpleFs);

typedef enum
{
   SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY,
   SIMPLE_FS_SDEVICE_EXCEPTION_BAD_AREA_OVERFLOW
} SimpleFsSDeviceException;

typedef enum
{
   SIMPLE_FS_SDEVICE_STATUS_OK,
   SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED,
   SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED
} SimpleFsSDeviceStatus;

SDEVICE_INIT_DATA_DECLARATION(SimpleFs)
{
   void (* ReadUInt64)(SDEVICE_HANDLE(SimpleFs) *handle,
                       const SimpleFsSDeviceSector *sector,
                       uintptr_t address,
                       uint64_t *value);
   void (* WriteUInt64)(SDEVICE_HANDLE(SimpleFs) *handle,
                        const SimpleFsSDeviceSector *sector,
                        uintptr_t address,
                        uint64_t value);
   void (* EraseSector)(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

#ifdef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   uint8_t (* UpdateCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, uint8_t crc, const void *data, size_t size);
   uint8_t (* ComputeCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);
   uint16_t (* UpdateCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t crc, const void *data, size_t size);
   uint16_t (* ComputeCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);
#endif

   SimpleFsSDeviceSector Sector$0;
   SimpleFsSDeviceSector Sector$1;
   bool IsMemoryErasingToZero;
};

SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs, init, parent, identifier, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFs, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(SimpleFs, TotalBadBlocksCount, size_t);
SDEVICE_GET_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value);

void SimpleFsSDeviceFormatMemory(SDEVICE_HANDLE(SimpleFs) *handle);
void SimpleFsSDeviceForceHistoryDeletion(SDEVICE_HANDLE(SimpleFs) *handle);
void SimpleFsSDeviceDeleteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileId);
size_t SimpleFsSDeviceGetMaxFileSize(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileId);
size_t SimpleFsSDeviceReadFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileId, void *data, size_t size);
void SimpleFsSDeviceWriteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileId, const void *data, size_t size);
