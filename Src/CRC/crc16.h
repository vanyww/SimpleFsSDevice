#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint16_t FlashFileSystemCrcType;

FlashFileSystemCrcType FlashFileSystemComputeCrc16(const void *, size_t);
FlashFileSystemCrcType FlashFileSystemUpdateCrc16(const void *, size_t, uint16_t);
