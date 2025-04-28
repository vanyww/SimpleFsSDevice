#include "../../Mock/Bindings/sdevice_core.h"
#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "unity_fixture.h"

#include <memory.h>

TEST_GROUP(Read);

TEST_SETUP(Read) { }
TEST_TEAR_DOWN(Read) { }

TEST(Read, FileWithIncompleteFilledLastDataBlock)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, fileData);

   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST(Read, FileWithFullyFilledDataBlocks)
{
   size_t fileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, fileData);

   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST(Read, NewFileVersionTwoTimes)
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

   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readFileData[fileDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL_UINT(fileDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, readFileData, fileDataSize);

   sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL_UINT(fileDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, readFileData, fileDataSize);
}

TEST_GROUP_RUNNER(Read)
{
   RUN_TEST_CASE(Read, FileWithIncompleteFilledLastDataBlock);
   RUN_TEST_CASE(Read, FileWithFullyFilledDataBlocks);
   RUN_TEST_CASE(Read, NewFileVersionTwoTimes);
}
