#pragma once

#include "SDeviceCore/interface.h"

#include <stddef.h>
#include <stdint.h>

#define __FLASH_FILE_SYSTEM_SDEVICE_MAX_ADDRESS (UINT16_MAX - 1)
#define __FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT 2
#define __FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE sizeof(uint64_t)

typedef uint16_t FlashFileSystemSDeviceAddress;

typedef struct
{
   void *SectorContext;
   intptr_t StartAddress;
   size_t Size;
} FlashFileSystemSDeviceSector;

typedef struct
{
   const FlashFileSystemSDeviceSector *Sector;
   intptr_t FirstFreeBlockAddress;
} FlashFileSystemSDeviceSectorDynamicData;

typedef enum
{
   FLASH_FILE_SYSTEM_SDEVICE_STATE_OK,
   FLASH_FILE_SYSTEM_SDEVICE_STATE_IO_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_SDEVICE_STATE_OUT_OF_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR,
   FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_SIZE_ERROR
} FlashFileSystemSDeviceState;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(FlashFileSystem);

typedef struct
{
   void *Context;
   bool (*TryReadFromFlash)(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, size_t, void *);
   bool (*TryWriteToFlash)(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, size_t, const void *);
   bool (*TryEraseFlashSector)(__SDEVICE_HANDLE(FlashFileSystem) *, const FlashFileSystemSDeviceSector *);
   FlashFileSystemSDeviceSector Sectors[__FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT];
   FlashFileSystemSDeviceAddress MaxUsedAddress;
} __SDEVICE_CONSTANT_DATA(FlashFileSystem);

typedef struct { } __SDEVICE_SETTINGS_DATA(FlashFileSystem);

typedef struct
{
   FlashFileSystemSDeviceSectorDynamicData *ActiveSector;
   FlashFileSystemSDeviceSectorDynamicData Sectors[__FLASH_FILE_SYSTEM_SDEVICE_SECTORS_COUNT];
   struct
   {
      intptr_t MemoryAddress;
      FlashFileSystemSDeviceAddress VariableAddress;
      bool IsDeleted;
      uint8_t VariableSize;
   } VariableDataCache;
   uint8_t BlockBuffer[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE];
} __SDEVICE_DYNAMIC_DATA(FlashFileSystem);

__SDEVICE_HANDLE_DEFINITION(FlashFileSystem);

__SDEVICE_INITIALIZE_INTERNALS_DECLARATION(FlashFileSystem,);

/* Satty's interface end */

FlashFileSystemSDeviceState FlashFileSystemSDeviceGetVariableSize(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                                  FlashFileSystemSDeviceAddress,
                                                                  size_t *);

FlashFileSystemSDeviceState FlashFileSystemSDeviceRead(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                       FlashFileSystemSDeviceAddress,
                                                       size_t,
                                                       void *);

FlashFileSystemSDeviceState FlashFileSystemSDeviceWrite(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                        FlashFileSystemSDeviceAddress,
                                                        size_t,
                                                        const void *);

FlashFileSystemSDeviceState FlashFileSystemSDeviceDelete(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                         FlashFileSystemSDeviceAddress);
