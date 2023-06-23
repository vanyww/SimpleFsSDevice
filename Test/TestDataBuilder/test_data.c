#include <memory.h>

#include "test_data.h"
#include "../Src/private.h"
#include "../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#define MAX_FILE_IMAGE_SIZE 32
#define MAX_FILE_SIZE 17
#define NUMBER_OF_FILES 2

#define CEIL_DIV_UINT(numerator, denominator)(                                                                          \
{                                                                                                                      \
   __auto_type _numerator = (numerator);                                                                               \
   __auto_type _denominator = (denominator);                                                                           \
   _numerator % _denominator > 0 ? _numerator / _denominator + 1 : _numerator / _denominator;                          \
})

typedef struct
{
   uint16_t FileDataSize;
   uint16_t FileImageDataSize;
   char FileData[MAX_FILE_SIZE];
   char FileImageData[MAX_FILE_IMAGE_SIZE];
}TestFile;

static TestFile TestFiles[NUMBER_OF_FILES];

uint16_t GetTestFileDataSize(TestFilesNames file)
{
   return TestFiles[file].FileDataSize;
}

uint16_t GetTestFileImageDataSize(TestFilesNames file)
{
   return TestFiles[file].FileImageDataSize;
}

void CopyTestFileData(void *buffer, TestFilesNames file)
{
   memcpy(buffer, TestFiles[file].FileData, TestFiles[file].FileDataSize);
}

void CopyTestFileImageData(void *buffer, TestFilesNames file)
{
   memcpy(buffer, TestFiles[file].FileImageData, TestFiles[file].FileImageDataSize);
}

static FileDataBlock CreateDataBlock(const void *data, size_t size, bool isMemoryErasingToZero)
{
   FileDataBlock block =
   {
      .Type = BLOCK_TYPE_FILE_DATA,
      .Data = { [0 ... SIZEOF_MEMBER(FileDataBlock, Data) - 1] = isMemoryErasingToZero ? 0 : UINT8_MAX }
   };
   memcpy(block.Data, data, size);

   return block;
}

static FileAreaTagBlock CreateFileAreaTagBlock(const void *fileData,
                                               size_t      fileSize,
                                               uint16_t    lastBlockDataSize,
                                               uint16_t    fileId)
{
   FileAreaTagBlock block =
   {
      .Type = BLOCK_TYPE_FILE_AREA_TAG,
      .FileAreaLength = CEIL_DIV_UINT(fileSize, SIZEOF_MEMBER(FileDataBlock, Data)),
      .LastFileBlockDataSize = lastBlockDataSize,
      .FileId = fileId,
      .FileCrc = TableCrc16SDeviceCompute(SimpleFsSDeviceInternalCrc16Handle, fileData, fileSize)
   };
   uint8_t* blockdata = ((ServiceBlock)block).BlockData;
   size_t size = sizeof(((ServiceBlock)block).BlockData);
   block.BlockCrc = TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, blockdata, size);

   return block;
}

static void InitializeFileWithSizeMultipleOfFileDataBlockDataSize(void)
{
   char fileData[] = "1111111111111";
   uint16_t fileSize = 14;
   uint16_t fileImageSize = 24;

   TestFile file =
   {
      .FileDataSize = fileSize,
      .FileImageDataSize = fileImageSize
   };
   memcpy(file.FileData, fileData, fileSize);

   uint16_t file1SizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);    // last block data size = 7 byte
   uint16_t lastBlockDataSize = (file1SizeRemainder != 0) ? file1SizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);

   FileDataBlock datablock1 = CreateDataBlock(&fileData[SIZEOF_MEMBER(FileDataBlock, Data)], lastBlockDataSize, true);
   memcpy(file.FileImageData, &datablock1, sizeof(Block));

   FileDataBlock datablock2 = CreateDataBlock(fileData, SIZEOF_MEMBER(FileDataBlock, Data), true);
   memcpy(&file.FileImageData[sizeof(Block)], &datablock2, sizeof(Block));

   FileAreaTagBlock fileAreaTagBlock = CreateFileAreaTagBlock(fileData, fileSize, lastBlockDataSize, 0);
   memcpy(&file.FileImageData[2*sizeof(Block)], &fileAreaTagBlock, sizeof(Block));

   TestFiles[FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE] = file;
}

static void InitializeFileWithSizeNotMultipleOfFileDataBlockDataSize(void)
{
   char fileData[] = "029750-185223854";
   uint16_t fileSize = 17;
   uint16_t fileImageSize = 32;

   TestFile file =
   {
      .FileDataSize = fileSize,
      .FileImageDataSize = fileImageSize
   };
   memcpy(file.FileData, fileData, fileSize);

   uint16_t file1SizeRemainder = fileSize % SIZEOF_MEMBER(FileDataBlock, Data);
   uint16_t lastBlockDataSize = (file1SizeRemainder != 0) ? file1SizeRemainder : SIZEOF_MEMBER(FileDataBlock, Data);

   FileDataBlock datablock1 = CreateDataBlock(&fileData[2*SIZEOF_MEMBER(FileDataBlock, Data)], lastBlockDataSize, true);
   memcpy(file.FileImageData, &datablock1, sizeof(Block));

   FileDataBlock datablock2 =
         CreateDataBlock(&fileData[SIZEOF_MEMBER(FileDataBlock, Data)], SIZEOF_MEMBER(FileDataBlock, Data), true);
   memcpy(&file.FileImageData[sizeof(Block)], &datablock2, sizeof(Block));

   FileDataBlock datablock3 = CreateDataBlock(fileData, SIZEOF_MEMBER(FileDataBlock, Data), true);
   memcpy(&file.FileImageData[2*sizeof(Block)], &datablock3, sizeof(Block));

   FileAreaTagBlock fileAreaTagBlock = CreateFileAreaTagBlock(fileData, fileSize, lastBlockDataSize, 1);
   memcpy(&file.FileImageData[3*sizeof(Block)], &fileAreaTagBlock, sizeof(Block));

   TestFiles[FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE] = file;
}

void CreateTestData(void)
{
   InitializeCrc16();
   InitializeCrc8();
   InitializeFileWithSizeMultipleOfFileDataBlockDataSize();
   InitializeFileWithSizeNotMultipleOfFileDataBlockDataSize();
}
