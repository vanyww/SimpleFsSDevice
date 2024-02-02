#pragma once

#include "../../../../../../private.h"

#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
extern SDEVICE_HANDLE(TableCrc8) *SimpleFsSDeviceInternalCrc8Handle;
extern SDEVICE_HANDLE(TableCrc16) *SimpleFsSDeviceInternalCrc16Handle;
#endif

static inline void InitializeCrc8(void)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if(SimpleFsSDeviceInternalCrc8Handle != NULL)
      return;

   SDEVICE_INIT_DATA(TableCrc8) crc8Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial          = 0x83,
      .InitialValue        = 0xFF,
      .OutputXorValue      = 0xFF,
      .IsReverse           = true
   };

   SDeviceHandleIdentifier identifier = SIMPLE_FS_SDEVICE_TABLE_CRC8_INTERNAL_SDEVICE_IDENTIFIER;
   SimpleFsSDeviceInternalCrc8Handle = SDEVICE_CREATE_HANDLE(TableCrc8)(&crc8Init, NULL, identifier, NULL);
#endif
}

static inline void InitializeCrc16(void)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if(SimpleFsSDeviceInternalCrc16Handle != NULL)
      return;

   SDEVICE_INIT_DATA(TableCrc16) crc16Init =
   {
      .ExternalLookupTable = NULL,
      .Polynomial          = 0xD175,
      .InitialValue        = 0xFFFF,
      .OutputXorValue      = 0xFFFF,
      .IsReverse           = true
   };

   SDeviceHandleIdentifier identifier = SIMPLE_FS_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER;
   SimpleFsSDeviceInternalCrc16Handle = SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, NULL, identifier, NULL);
#endif
}

__attribute__((always_inline))
static inline uint8_t UpdateCrc8(ThisHandle *handle, uint8_t crc, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc8SDeviceUpdate(SimpleFsSDeviceInternalCrc8Handle, crc, data, size);
#else
   return handle->Init.UpdateCrc8(handle, crc, data, size);
#endif
}

__attribute__((always_inline))
static inline uint8_t ComputeCrc8(ThisHandle *handle, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, data, size);
#else
   return handle->Init.ComputeCrc8(handle, data, size);
#endif
}

__attribute__((always_inline))
static inline uint16_t UpdateCrc16(ThisHandle *handle, uint16_t crc, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc16SDeviceUpdate(SimpleFsSDeviceInternalCrc16Handle, crc, data, size);
#else
   return handle->Init.UpdateCrc16(handle, crc, data, size);
#endif
}

__attribute__((always_inline))
static inline uint16_t ComputeCrc16(ThisHandle *handle, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc16SDeviceCompute(SimpleFsSDeviceInternalCrc16Handle, data, size);
#else
   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
