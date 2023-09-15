#include "../../Mock/Errors/errors.h"
#include "../../Mock/SimpleFs/Application/application.h"
#include "../../Mock/SimpleFs/Data/TestDataBuilder/data_builder.h"

#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <memory.h>
#include "unity_fixture.h"

TEST_GROUP(ReadWrite);

TEST_SETUP(ReadWrite) {}
TEST_TEAR_DOWN(ReadWrite) {}

TEST(ReadWrite, WriteFileWithFullyFilledDataBlocks)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   size_t fileImageDataSize = GetTestFileImageDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   char fileImageData[fileImageDataSize];
   CopyTestFileData(fileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   CopyTestFileImageData(fileImageData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   CREATE_SIMPLE_FS_APPLICATION(48, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MEMORY_SECTORS(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(ReadWrite, WriteFileWithIncompleteFilledLastDataBlock)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   size_t fileImageDataSize = GetTestFileImageDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   char fileImageData[fileImageDataSize];
   CopyTestFileData(fileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   CopyTestFileImageData(fileImageData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(48, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 1, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MEMORY_SECTORS(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(ReadWrite, ReadFileWithIncompleteFilledLastDataBlock)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(48, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST(ReadWrite, ReadFileWithFullyFilledDataBlocks)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   CREATE_SIMPLE_FS_APPLICATION(48, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST(ReadWrite, WriteNewFileVersion)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t fileNewVersionDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileNewVersionData[fileNewVersionDataSize];
   CopyTestFileData(fileNewVersionData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, fileNewVersionData, fileNewVersionDataSize);
}

TEST(ReadWrite, WriteNewFileVersionWithSmallerSize)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   size_t fileNewVersionDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char fileNewVersionData[fileNewVersionDataSize];
   CopyTestFileData(fileNewVersionData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileDataSize);

   TEST_ASSERT_EQUAL(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, fileNewVersionData, fileNewVersionDataSize);
}

TEST(ReadWrite, ReadNewFileVersionTwoTimes)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);

   size_t fileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t fileNewVersionDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char fileNewVersionData[fileNewVersionDataSize];
   CopyTestFileData(fileNewVersionData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readFileData[fileDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);
   TEST_ASSERT_EQUAL(fileDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, readFileData, fileDataSize);
   sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);
   TEST_ASSERT_EQUAL(fileDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(readFileData, readFileData, fileDataSize);
}

TEST(ReadWrite, WriteFileWithShortageMemory)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(true);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(48, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
   TEST_FAIL_MESSAGE("Test fail, no exception was thrown");
}

TEST_GROUP_RUNNER(ReadWrite)
{
   RUN_TEST_CASE(ReadWrite, WriteFileWithFullyFilledDataBlocks);
   RUN_TEST_CASE(ReadWrite, WriteFileWithIncompleteFilledLastDataBlock);
   RUN_TEST_CASE(ReadWrite, ReadFileWithIncompleteFilledLastDataBlock);
   RUN_TEST_CASE(ReadWrite, ReadFileWithFullyFilledDataBlocks);
   RUN_TEST_CASE(ReadWrite, WriteNewFileVersion);
   RUN_TEST_CASE(ReadWrite, WriteFileWithShortageMemory);
   RUN_TEST_CASE(ReadWrite, WriteNewFileVersionWithSmallerSize);
   RUN_TEST_CASE(ReadWrite, ReadNewFileVersionTwoTimes);
}
