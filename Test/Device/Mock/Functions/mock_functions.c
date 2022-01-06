#include "mock_functions.h"

#include <memory.h>

uint8_t MockMemorySectors[__FLASH_FILE_SYSTEM_SECTORS_COUNT][__MOCK_MEMORY_SECTOR_SIZE];

bool TryReadFromMemory(__SDEVICE_HANDLE(FlashFileSystem) *handle, intptr_t address, size_t size, void *data)
{
   memcpy(data, (void *)address, size);
   return true;
}

bool TryWriteToMemory(__SDEVICE_HANDLE(FlashFileSystem) *handle, intptr_t address, size_t size, const void *data)
{
   memcpy((void *)address, data, size);
   return true;
}

bool TryEraseMemorySector(__SDEVICE_HANDLE(FlashFileSystem) *handle, const FlashFileSystemSector *sector)
{
   memset((void *)sector->StartAddress, 0xFF, __MOCK_MEMORY_SECTOR_SIZE);
   return true;
}
