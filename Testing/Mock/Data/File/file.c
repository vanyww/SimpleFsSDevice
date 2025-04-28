#include "file.h"

#include "../../../../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"
#include "../../../../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include "SDeviceCore/common.h"

#include <memory.h>

#define MAX_FILE_IMAGE_SIZE 32
#define MAX_FILE_SIZE       17
#define FILES_COUNT         2

typedef struct
{
   size_t FileSize;
   size_t FileImageSize;
   char   FileData[MAX_FILE_SIZE];
   char   FileImageData[MAX_FILE_IMAGE_SIZE];
} MockFile;

static MockFile Files[FILES_COUNT] =
{
   {
      .FileSize      = 14,
      .FileImageSize = 24,
      .FileData      = "1111111111111",
      .FileImageData =
      {
         49, 49, 49, 49, 49, 49, 0, 34, 49, 49, 49, 49,
         49, 49, 49, 34, 28, 2, 7, 0, 0, -118, -106, -120
      }
   },

   {
      .FileSize      = 17,
      .FileImageSize = 32,
      .FileData      = "029750-185223854",
      .FileImageData =
      {
         53, 52, 0, 0, 0, 0, 0, 34, 49, 56, 53, 50,
         50, 51, 56, 34, 48, 50, 57, 55, 53, 48, 45,
         34, -111, 3, 3, 1, 0, -36, -71, -120
      }
   }
};

size_t MockGetFileSize(MockFileType type)
{
   return Files[type].FileSize;
}

size_t MockGetFileImageSize(MockFileType type)
{
   return Files[type].FileImageSize;
}

void MockGetFileData(MockFileType type, void *buffer)
{
   memcpy(buffer, Files[type].FileData, Files[type].FileSize);
}

void MockGetFileImageData(MockFileType type, void *buffer)
{
   memcpy(buffer, Files[type].FileImageData, Files[type].FileImageSize);
}
