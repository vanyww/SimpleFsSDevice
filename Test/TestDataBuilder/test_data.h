#pragma once

#include <stdint.h>

typedef enum
{
   FILE_WITH_FULLY_FILLED_DATA_BLOCKS,
   FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK
} TestFilesNames;

void CreateTestData(void);

void CopyTestFileData(void *buffer, TestFilesNames file);

void CopyTestFileImageData(void *buffer, TestFilesNames file);

uint16_t GetTestFileImageDataSize(TestFilesNames file);

uint16_t GetTestFileDataSize(TestFilesNames file);
