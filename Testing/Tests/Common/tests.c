#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "unity_fixture.h"

TEST_GROUP(Common);

TEST_SETUP(Common) { }
TEST_TEAR_DOWN(Common) {}

TEST(Common, DeleteFile)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFirstFile);
   TEST_ASSERT_EQUAL(secondFileDataSize, sizeOfReadSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Common, GetMaxFileSize)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, secondFileData, secondFileDataSize);

   size_t maxFileSize = SimpleFsSDeviceGetMaxFileSize(handle, 0);

   TEST_ASSERT_EQUAL_UINT(secondFileDataSize, maxFileSize);
}

TEST(Common, DeleteSomeFileVersion)
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

   SimpleFsSDeviceDeleteFile(handle, 0);

   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL_UINT(0, sizeOfReadFile);
}

TEST(Common, DeleteFileWithShortageMemory)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 72);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   char secondReadFileData[secondFileDataSize];

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(0, sizeOfReadFirstFile);
   TEST_ASSERT_EQUAL_UINT(secondFileDataSize, sizeOfReadSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Common, FormatMemory)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   SimpleFsSDeviceFormatMemory(handle);

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(0, sizeOfReadFirstFile);
   TEST_ASSERT_EQUAL_UINT(0, sizeOfReadSecondFile);
}

TEST(Common, ForceHistoryDeletion)
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

   SimpleFsSDeviceForceHistoryDeletion(handle);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL_UINT(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileNewVersionData, readFileData, fileNewVersionDataSize);
}

TEST_GROUP_RUNNER(Common)
{
   RUN_TEST_CASE(Common, DeleteFile);
   RUN_TEST_CASE(Common, GetMaxFileSize);
   RUN_TEST_CASE(Common, DeleteSomeFileVersion);
   RUN_TEST_CASE(Common, DeleteFileWithShortageMemory);
   RUN_TEST_CASE(Common, FormatMemory);
   RUN_TEST_CASE(Common, ForceHistoryDeletion);
}
