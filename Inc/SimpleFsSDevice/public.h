#pragma once

#include "config.h"
#include "dependensies.h"

#define SIMPLE_FS_SDEVICE_VERSION_MAJOR 4
#define SIMPLE_FS_SDEVICE_VERSION_MINOR 0
#define SIMPLE_FS_SDEVICE_VERSION_PATCH 0

typedef struct
{
   void  *Context;
   size_t Size;
} SimpleFsSDeviceSector;

typedef enum
{
   SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY,
   SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW
} SimpleFsSDevicePanic;

SDEVICE_HANDLE_FORWARD_DECLARATION(SimpleFs);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(SimpleFs);

SDEVICE_INIT_DATA_DECLARATION(SimpleFs)
{
   void (* ReadUInt64)(
         SDEVICE_HANDLE(SimpleFs)    *handle,
         const SimpleFsSDeviceSector *sector,
         uintptr_t                    address,
         uint64_t                    *value);

   void (* WriteUInt64)(
         SDEVICE_HANDLE(SimpleFs)    *handle,
         const SimpleFsSDeviceSector *sector,
         uintptr_t                    address,
         uint64_t                     value);

   void (* EraseSector)(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

#ifdef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   uint8_t (* UpdateCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, uint8_t crc, const void *data, size_t size);
   uint8_t (* ComputeCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);
   uint16_t (* UpdateCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t crc, const void *data, size_t size);
   uint16_t (* ComputeCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);
#endif

   SimpleFsSDeviceSector PrimarySector;
   SimpleFsSDeviceSector AuxiliarySector;
   bool                  IsMemoryErasingToZero;
};

SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs, init, context);
SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFs, handlePointer);

SDEVICE_PROPERTY_TYPE_DECLARATION(SimpleFs, TotalBadBlocksCount, size_t);
SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value);

void SimpleFsSDeviceFormatMemory(SDEVICE_HANDLE(SimpleFs) *handle);
void SimpleFsSDeviceForceHistoryDeletion(SDEVICE_HANDLE(SimpleFs) *handle);
void SimpleFsSDeviceDeleteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx);
size_t SimpleFsSDeviceGetMaxFileSize(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx);
size_t SimpleFsSDeviceReadFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx, void *data, size_t size);
void SimpleFsSDeviceWriteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx, const void *data, size_t size);
