#pragma once

#include "../../../../../../private.h"

#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
extern SDEVICE_HANDLE(TableCrc8) *$SimpleFsSDeviceCrc8Handle;
extern SDEVICE_HANDLE(TableCrc16) *$SimpleFsSDeviceCrc16Handle;
#endif

static inline void InitializeCrc8(void)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if($SimpleFsSDeviceCrc8Handle)
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
   $SimpleFsSDeviceCrc8Handle = SDEVICE_CREATE_HANDLE(TableCrc8)(&crc8Init, NULL, identifier, NULL);
#endif
}

static inline void InitializeCrc16(void)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   if($SimpleFsSDeviceCrc16Handle)
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
   $SimpleFsSDeviceCrc16Handle = SDEVICE_CREATE_HANDLE(TableCrc16)(&crc16Init, NULL, identifier, NULL);
#endif
}

__attribute__((always_inline))
static inline uint8_t UpdateCrc8(ThisHandle *handle, uint8_t crc, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc8SDeviceUpdate($SimpleFsSDeviceCrc8Handle, crc, data, size);
#else
   return handle->Init.UpdateCrc8(handle, crc, data, size);
#endif
}

__attribute__((always_inline))
static inline uint8_t ComputeCrc8(ThisHandle *handle, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc8SDeviceCompute($SimpleFsSDeviceCrc8Handle, data, size);
#else
   return handle->Init.ComputeCrc8(handle, data, size);
#endif
}

__attribute__((always_inline))
static inline uint16_t UpdateCrc16(ThisHandle *handle, uint16_t crc, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc16SDeviceUpdate($SimpleFsSDeviceCrc16Handle, crc, data, size);
#else
   return handle->Init.UpdateCrc16(handle, crc, data, size);
#endif
}

__attribute__((always_inline))
static inline uint16_t ComputeCrc16(ThisHandle *handle, const void *data, size_t size)
{
#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   return TableCrc16SDeviceCompute($SimpleFsSDeviceCrc16Handle, data, size);
#else
   return handle->Init.ComputeCrc16(handle, data, size);
#endif
}
