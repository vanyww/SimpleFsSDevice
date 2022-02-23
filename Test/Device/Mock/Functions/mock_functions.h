#pragma once

#include "FlashFileSystemSDevice/core.h"

#define __MOCK_MEMORY_SECTOR_SIZE (8 * 8) /* 8 blocks */

extern uint8_t MockMemorySectors[__FLASH_FILE_SYSTEM_SECTORS_COUNT][__MOCK_MEMORY_SECTOR_SIZE];

bool TryReadFromMemory(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, FlashFileSystemBlockValue *);
bool TryWriteToMemory(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t, const FlashFileSystemBlockValue *);
bool TryEraseMemorySector(__SDEVICE_HANDLE(FlashFileSystem) *, const FlashFileSystemSector *);
