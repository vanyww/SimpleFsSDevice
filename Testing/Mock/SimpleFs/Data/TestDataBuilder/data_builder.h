#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
   FILE_WITH_FULLY_FILLED_DATA_BLOCKS,
   FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK
} TestFilesNames;

void CreateTestData(void);
size_t GetTestFileDataSize(TestFilesNames file);
size_t GetTestFileImageDataSize(TestFilesNames file);
void CopyTestFileData(void *buffer, TestFilesNames file);
void CopyTestFileImageData(void *buffer, TestFilesNames file);
