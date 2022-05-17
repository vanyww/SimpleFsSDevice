#pragma once

#include "SDeviceCore/interface.h"

#define __FLASH_FILE_SYSTEM_MAX_ADDRESS (UINT16_MAX - 1)
#define __FLASH_FILE_SYSTEM_SECTORS_COUNT 2

typedef uint64_t FlashFileSystemBlockValue;
typedef uint16_t FlashFileSystemAddress;

typedef struct
{
   void *Context;
   uintptr_t StartAddress;
   size_t Size;
} FlashFileSystemSector;

typedef struct
{
   size_t SectorIndex;
   uintptr_t WriteCursor;
   uintptr_t ReadCursor;
} FlashFileSystemIterator;

typedef enum
{
   FLASH_FILE_SYSTEM_STATUS_OK,
   FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_STATUS_OUT_OF_MEMORY_ERROR,
   FLASH_FILE_SYSTEM_STATUS_FILE_NOT_FOUND_ERROR,
   FLASH_FILE_SYSTEM_STATUS_FILE_SIZE_ERROR
} FlashFileSystemStatus;

typedef struct
{
   FlashFileSystemAddress Address;
   uintptr_t MemoryAddress;
   uint8_t Size;
   bool IsDeleted;
} FlashFileSystemFileDataCache;

/* Satty's interface start */

__SDEVICE_HANDLE_FORWARD_DECLARATION(FlashFileSystem);

typedef struct
{
   bool (* TryReadBlock)(__SDEVICE_HANDLE(FlashFileSystem) *, uintptr_t, FlashFileSystemBlockValue *);
   bool (* TryWriteBlock)(__SDEVICE_HANDLE(FlashFileSystem) *, uintptr_t, const FlashFileSystemBlockValue *);
   bool (* TryEraseSector)(__SDEVICE_HANDLE(FlashFileSystem) *, const FlashFileSystemSector *);
   FlashFileSystemSector Sectors[__FLASH_FILE_SYSTEM_SECTORS_COUNT];
   FlashFileSystemAddress MaxUsedAddress;
} __SDEVICE_INIT_DATA(FlashFileSystem);

typedef struct
{
   size_t ActiveIteratorIndex;
   FlashFileSystemIterator Iterators[__FLASH_FILE_SYSTEM_SECTORS_COUNT];
   FlashFileSystemFileDataCache FileDataCache;
} __SDEVICE_RUNTIME_DATA(FlashFileSystem);

__SDEVICE_HANDLE_DEFINITION(FlashFileSystem);

__SDEVICE_INITIALIZE_HANDLE_DECLARATION(FlashFileSystem,);

typedef enum
{
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_READ_FAIL       = 0x01,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRITE_FAIL      = 0x02,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_ERASE_FAIL      = 0x03,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_OUT_OF_MEMORY   = 0x04,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_CORRUPTED_STATE = 0x05,
   FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRONG_FILE_SIZE = 0x06
} FlashFileSystemRuntimeError;

/* Satty's interface end */

FlashFileSystemStatus FlashFileSystemGetFileSize(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress, size_t *);
FlashFileSystemStatus FlashFileSystemRead(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress, size_t, void *);
FlashFileSystemStatus FlashFileSystemDelete(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress);
FlashFileSystemStatus FlashFileSystemWrite(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           FlashFileSystemAddress,
                                           size_t,
                                           const void *);
