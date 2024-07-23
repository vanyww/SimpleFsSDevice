#include "data_builder.h"

#include "SDeviceCore/common.h"
#include "../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <memory.h>

#define MAX_FILE_IMAGE_SIZE 32
#define MAX_FILE_SIZE 17
#define NUMBER_OF_FILES 2

typedef struct
{
   size_t FileDataSize;
   size_t FileImageDataSize;
   char   FileData[MAX_FILE_SIZE];
   char   FileImageData[MAX_FILE_IMAGE_SIZE];
} TestFile;

static TestFile Files[NUMBER_OF_FILES];

size_t GetTestFileDataSize(TestFilesNames file)
{
   return Files[file].FileDataSize;
}

size_t GetTestFileImageDataSize(TestFilesNames file)
{
   return Files[file].FileImageDataSize;
}

void CopyTestFileData(void *buffer, TestFilesNames file)
{
   memcpy(buffer, Files[file].FileData, Files[file].FileDataSize);
}

void CopyTestFileImageData(void *buffer, TestFilesNames file)
{
   memcpy(buffer, Files[file].FileImageData, Files[file].FileImageDataSize);
}

static void InitializeFileWithFullyFilledDataBlocks(void)
{
   char fileData[] = "1111111111111";
   size_t fileSize = 14;
   size_t fileImageSize = 24;

   TestFile file =
   {
      .FileDataSize = fileSize,
      .FileImageDataSize = fileImageSize
   };
   memcpy(file.FileData, fileData, fileSize);

   uint8_t file1SizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);    // last block data size = 7 byte
   uint8_t lastBlockDataSize = (file1SizeRemainder != 0) ? file1SizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);

   char dataBlock1[sizeof(Block)] = { 0 };
   memcpy(dataBlock1, &fileData[SIZEOF_MEMBER(FileDataBlock, Data)], lastBlockDataSize);
   dataBlock1[7] = BLOCK_TYPE_FILE_DATA;
   memcpy(file.FileImageData, &dataBlock1, sizeof(Block));

   char dataBlock2[sizeof(Block)] = { 0 };
   memcpy(dataBlock2, fileData, SIZEOF_MEMBER(FileDataBlock, Data));
   dataBlock2[7] = BLOCK_TYPE_FILE_DATA;
   memcpy(&file.FileImageData[sizeof(Block)], &dataBlock2, sizeof(Block));

   char fileAreaTagBlock[sizeof(Block)] = { 0 };
   fileAreaTagBlock[1] = 2;   // file area length
   fileAreaTagBlock[2] = 7;   // last block data size
   uint16_t fileIdx = 0;
   memcpy(&fileAreaTagBlock[3], &fileIdx, sizeof(uint16_t));
   uint16_t fileCrc = TableCrc16SDeviceCompute($SimpleFsSDeviceCrc16Handle, fileData, fileSize);
   memcpy(&fileAreaTagBlock[5], &fileCrc, sizeof(uint16_t));
   fileAreaTagBlock[7] = BLOCK_TYPE_FILE_AREA_TAG;
   fileAreaTagBlock[0] =
      TableCrc8SDeviceCompute($SimpleFsSDeviceCrc8Handle, &fileAreaTagBlock[1], sizeof(Block) - sizeof(uint8_t));
   memcpy(&file.FileImageData[2*sizeof(Block)], &fileAreaTagBlock, sizeof(Block));

   Files[FILE_WITH_FULLY_FILLED_DATA_BLOCKS] = file;
}

static void InitializeFileWithIncompleteFilledLastDataBlock(void)
{
   char fileData[] = "029750-185223854";
   size_t fileSize = 17;
   size_t fileImageSize = 32;

   TestFile file =
   {
      .FileDataSize = fileSize,
      .FileImageDataSize = fileImageSize
   };
   memcpy(file.FileData, fileData, fileSize);

   uint8_t file1SizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);
   uint8_t lastBlockDataSize = (file1SizeRemainder != 0) ? file1SizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);

   char dataBlock1[sizeof(Block)] = { 0 };
   memcpy(dataBlock1, &fileData[2*SIZEOF_MEMBER(FileDataBlock, Data)], lastBlockDataSize);
   dataBlock1[7] = BLOCK_TYPE_FILE_DATA;
   memcpy(file.FileImageData, &dataBlock1, sizeof(Block));

   char dataBlock2[sizeof(Block)] = { 0 };
   memcpy(dataBlock2, &fileData[SIZEOF_MEMBER(FileDataBlock, Data)], SIZEOF_MEMBER(FileDataBlock, Data));
   dataBlock2[7] = BLOCK_TYPE_FILE_DATA;
   memcpy(&file.FileImageData[sizeof(Block)], &dataBlock2, sizeof(Block));

   char dataBlock3[sizeof(Block)] = { 0 };
   memcpy(dataBlock3, &fileData, SIZEOF_MEMBER(FileDataBlock, Data));
   dataBlock3[7] = BLOCK_TYPE_FILE_DATA;
   memcpy(&file.FileImageData[2*sizeof(Block)], &dataBlock3, sizeof(Block));

   char fileAreaTagBlock[sizeof(Block)] = { 0 };
   fileAreaTagBlock[1] = 3;   // file area length
   fileAreaTagBlock[2] = 3;   // last block data size
   uint16_t fileIdx = 1;
   memcpy(&fileAreaTagBlock[3], &fileIdx, sizeof(uint16_t));
   uint16_t fileCrc = TableCrc16SDeviceCompute($SimpleFsSDeviceCrc16Handle, fileData, fileSize);
   memcpy(&fileAreaTagBlock[5], &fileCrc, sizeof(uint16_t));
   fileAreaTagBlock[7] = BLOCK_TYPE_FILE_AREA_TAG;
   fileAreaTagBlock[0] =
      TableCrc8SDeviceCompute($SimpleFsSDeviceCrc8Handle, &fileAreaTagBlock[1], sizeof(Block) - sizeof(uint8_t));
   memcpy(&file.FileImageData[3*sizeof(Block)], &fileAreaTagBlock, sizeof(Block));

   Files[FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK] = file;
}

void CreateTestData(void)
{
   InitializeCrc16();
   InitializeCrc8();
   InitializeFileWithFullyFilledDataBlocks();
   InitializeFileWithIncompleteFilledLastDataBlock();
}
