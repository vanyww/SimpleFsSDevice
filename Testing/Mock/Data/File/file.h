#pragma once

#include "../../../../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <stdbool.h>
#include <stddef.h>

typedef enum
{
   MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS,
   MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK
} MockFileType;

size_t MockGetFileSize(MockFileType type);
size_t MockGetFileImageSize(MockFileType type);
void MockGetFileData(MockFileType type, void *buffer);
void MockGetFileImageData(MockFileType type, void *buffer);
