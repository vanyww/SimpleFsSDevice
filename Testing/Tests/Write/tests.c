#include "../../Mock/Bindings/sdevice_core.h"
#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "unity_fixture.h"

#include <memory.h>

static const void *PanicHandle = NULL;
static SDevicePanic Panic = UINT16_MAX;

static void TestPanicHandler(const void *handle, SDevicePanic panic)
{
   PanicHandle = handle;
   Panic = panic;

   TEST_MESSAGE("OK");
}

TEST_GROUP(Write);

TEST_SETUP(Write) { }
TEST_TEAR_DOWN(Write) { }

TEST(Write, FileWithFullyFilledDataBlocks)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, fileData);

   size_t fileImageDataSize = MockGetFileImageSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char fileImageData[fileImageDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, fileImageData);

   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MOCK_SECTORS_DATA(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(Write, FileWithIncompleteFilledLastDataBlock)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, fileData);

   size_t fileImageDataSize = MockGetFileImageSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileImageData[fileImageDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, fileImageData);

   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 1, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MOCK_SECTORS_DATA(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(Write, NewFileVersion)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, fileData);

   size_t fileNewVersionDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileNewVersionData[fileNewVersionDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, fileNewVersionData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL_UINT(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, fileNewVersionData, fileNewVersionDataSize);
}

TEST(Write, NewFileVersionWithSmallerSize)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, fileData);

   size_t fileNewVersionDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char fileNewVersionData[fileNewVersionDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, fileNewVersionData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileDataSize);

   TEST_ASSERT_EQUAL_UINT(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, fileNewVersionData, fileNewVersionDataSize);
}

TEST(Write, FileWithShortageMemory)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

   SetPanicHandler(TestPanicHandler);

   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   ResetPanicHandler();

   TEST_ASSERT_EQUAL(handle, PanicHandle);
   TEST_ASSERT_EQUAL_INT16(SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY, Panic);
}

TEST_GROUP_RUNNER(Write)
{
   RUN_TEST_CASE(Write, FileWithFullyFilledDataBlocks);
   RUN_TEST_CASE(Write, FileWithIncompleteFilledLastDataBlock);
   RUN_TEST_CASE(Write, NewFileVersion);
   RUN_TEST_CASE(Write, NewFileVersionWithSmallerSize);
   RUN_TEST_CASE(Write, FileWithShortageMemory);
}
