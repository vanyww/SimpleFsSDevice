#pragma once

#include "../../../../../../private.h"

#include "SDeviceCore/errors.h"

#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
#include "TableCrcSDevice/public_crc16.h"
#include "TableCrcSDevice/public_crc8.h"

#if (TABLE_CRC_SDEVICE_VERSION_MAJOR != 3) || (TABLE_CRC_SDEVICE_VERSION_MINOR < 1)
#error Table CRC SDevice version is incorrect.
#endif

#ifndef SIMPLE_FS_SDEVICE_TABLE_CRC8_INTERNAL_SDEVICE_IDENTIFIER
#define SIMPLE_FS_SDEVICE_TABLE_CRC8_INTERNAL_SDEVICE_IDENTIFIER 0
#endif

#ifndef SIMPLE_FS_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER
#define SIMPLE_FS_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER 0
#endif

extern SDEVICE_HANDLE(TableCrc8) *SimpleFsSDeviceInternalCrc8Handle;
extern SDEVICE_HANDLE(TableCrc16) *SimpleFsSDeviceInternalCrc16Handle;
#endif

static inline void InitializeCrc8(void)
{
#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if(SimpleFsSDeviceInternalCrc8Handle != NULL)
      return;

   SDEVICE_INIT_DATA(TableCrc8) crc8Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial = 0x83,
      .InitialValue = 0xFF,
      .OutputXorValue = 0xFF,
      .IsReverse = true
   };

   SDeviceHandleIdentifier identifier = SIMPLE_FS_SDEVICE_TABLE_CRC8_INTERNAL_SDEVICE_IDENTIFIER;
   SimpleFsSDeviceInternalCrc8Handle = SDEVICE_CREATE_HANDLE(TableCrc8)(&crc8Init, NULL, identifier, NULL);
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc8Handle != NULL);
#endif
}

static inline void InitializeCrc16(void)
{
#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if(SimpleFsSDeviceInternalCrc16Handle != NULL)
      return;

   SDEVICE_INIT_DATA(TableCrc16) crc16Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial = 0xD175,
      .InitialValue = 0xFFFF,
      .OutputXorValue = 0xFFFF,
      .IsReverse = true
   };

   SDeviceHandleIdentifier identifier = SIMPLE_FS_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER;
   SimpleFsSDeviceInternalCrc16Handle = SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, NULL, identifier, NULL);
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc16Handle != NULL);
#endif
}

static inline uint8_t UpdateCrc8(ThisHandle *handle, uint8_t crc, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc8Handle != NULL);

   return TableCrc8SDeviceUpdate(SimpleFsSDeviceInternalCrc8Handle, crc, data, size);
#else
   SDeviceDebugAssert(handle->Init.UpdateCrc8 != NULL);

   return handle->Init.UpdateCrc8(handle, crc, data, size);
#endif
}

static inline uint8_t ComputeCrc8(ThisHandle *handle, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc8Handle != NULL);

   return TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, data, size);
#else
   SDeviceDebugAssert(handle->Init.ComputeCrc8 != NULL);

   return handle->Init.ComputeCrc8(handle, data, size);
#endif
}

static inline uint16_t UpdateCrc16(ThisHandle *handle, uint16_t crc, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc16Handle != NULL);

   return TableCrc16SDeviceUpdate(SimpleFsSDeviceInternalCrc16Handle, crc, data, size);
#else
   SDeviceDebugAssert(handle->Init.UpdateCrc16 != NULL);

   return handle->Init.UpdateCrc16(handle, crc, data, size);
#endif
}

static inline uint16_t ComputeCrc16(ThisHandle *handle, const void *data, size_t size)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(data != NULL || size == 0);

#ifndef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceDebugAssert(SimpleFsSDeviceInternalCrc16Handle != NULL);

   return TableCrc16SDeviceCompute(SimpleFsSDeviceInternalCrc16Handle, data, size);
#else
   SDeviceDebugAssert(handle->Init.ComputeCrc16 != NULL);

   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
