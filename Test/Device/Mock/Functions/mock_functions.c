#include "mock_functions.h"

#include <memory.h>

uint8_t MockMemorySectors[__FLASH_FILE_SYSTEM_SECTORS_COUNT][__MOCK_MEMORY_SECTOR_SIZE];

bool TryReadFromMemory(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                       uintptr_t address,
                       FlashFileSystemBlockValue *data)
{
   memcpy(data, (void *)address, sizeof(FlashFileSystemBlockValue));
   return true;
}

bool TryWriteToMemory(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                      uintptr_t address,
                      const FlashFileSystemBlockValue *data)
{
   memcpy((void *)address, data, sizeof(FlashFileSystemBlockValue));
   return true;
}

bool TryEraseMemorySector(__SDEVICE_HANDLE(FlashFileSystem) *handle, const FlashFileSystemSector *sector)
{
   memset((void *)sector->StartAddress, 0xFF, __MOCK_MEMORY_SECTOR_SIZE);
   return true;
}
