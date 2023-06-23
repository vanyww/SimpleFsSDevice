#pragma once

#include <stdint.h>

typedef enum
{
   FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE,
   FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE,
} TestFilesNames;

void CreateTestData(void);

void CopyTestFileData(void *buffer, TestFilesNames file);

void CopyTestFileImageData(void *buffer, TestFilesNames file);

uint16_t GetTestFileImageDataSize(TestFilesNames file);

uint16_t GetTestFileDataSize(TestFilesNames file);
