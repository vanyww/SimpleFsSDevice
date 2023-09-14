#pragma once

#include "stddef.h"

size_t GetTestBadBlocksNumbersArraySize(size_t sectorNumber);
size_t* GetTestBadBlocksNumbersArrayPtr(size_t sectorNumber);
void SetTestBadBlocksConfig(size_t *ptr, size_t size, size_t sectorIndex);
