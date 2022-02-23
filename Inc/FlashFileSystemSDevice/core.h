#pragma once

#include "SDeviceCore/interface.h"

#define __FLASH_FILE_SYSTEM_MAX_ADDRESS (UINT16_MAX - 1)
#define __FLASH_FILE_SYSTEM_SECTORS_COUNT 2

typedef uint16_t FlashFileSystemAddress;

typedef struct
{
   void *Context;
   intptr_t StartAddress;
   size_t Size;
} FlashFileSystemSector;

typedef struct
{
   const FlashFileSystemSector *Sector;
   intptr_t WriteCursor;
   intptr_t ReadCursor;
} FlashFileSystemIterator;

typedef enum
{
   FLASH_FILE_SYSTEM_STATUS_OK,
   FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_STATUS_OUT_OF_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR,
   FLASH_FILE_SYSTEM_STATUS_VALUE_SIZE_ERROR
} FlashFileSystemStatus;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(FlashFileSystem);

typedef struct
{
   bool (* TryRead)(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, size_t, void *);
   bool (* TryWrite)(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, size_t, const void *);
   bool (* TryEraseSector)(__SDEVICE_HANDLE(FlashFileSystem) *, const FlashFileSystemSector *);
   FlashFileSystemSector Sectors[__FLASH_FILE_SYSTEM_SECTORS_COUNT];
   FlashFileSystemAddress MaxUsedAddress;
} __SDEVICE_INIT_DATA(FlashFileSystem);

typedef struct
{
   FlashFileSystemIterator *ActiveIterator;
   FlashFileSystemIterator Iterators[__FLASH_FILE_SYSTEM_SECTORS_COUNT];
   struct
   {
      FlashFileSystemAddress Address;
      intptr_t MemoryAddress;
      uint8_t Size;
      bool IsDeleted;
   } VariableDataCache;
} __SDEVICE_RUNTIME_DATA(FlashFileSystem);

__SDEVICE_HANDLE_DEFINITION(FlashFileSystem);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem,);

typedef enum
{
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_READ_FAIL           = 0x01,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRITE_FAIL          = 0x02,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_ERASE_FAIL          = 0x03,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_OUT_OF_MEMORY       = 0x04,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_CORRUPTED_STATE     = 0x05,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRONG_VARIABLE_SIZE = 0x06
} FlashFileSystemRuntimeError;

/* Satty's interface end */

FlashFileSystemStatus FlashFileSystemGetVariableSize(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                     FlashFileSystemAddress,
                                                     size_t *);

FlashFileSystemStatus FlashFileSystemWrite(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           FlashFileSystemAddress,
                                           size_t,
                                           const void *);

FlashFileSystemStatus FlashFileSystemRead(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress, size_t, void *);
FlashFileSystemStatus FlashFileSystemDelete(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress);
