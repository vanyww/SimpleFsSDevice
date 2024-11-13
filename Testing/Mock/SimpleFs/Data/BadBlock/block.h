#pragma once

#include "stddef.h"

size_t GetBadBlocksNumbersArraySize(size_t sectorNumber);
size_t* GetBadBlocksNumbersArrayPtr(size_t sectorNumber);
void SetBadBlocksNumbers(size_t *ptr, size_t size, size_t sectorIndex);
