#include "../../Mock/Errors/errors.h"
#include "../../Mock/SimpleFs/Application/application.h"
#include "../../Mock/SimpleFs/Data/TestDataBuilder/data_builder.h"

#include "unity_fixture.h"

TEST_GROUP(CommonSimpleFs);

TEST_SETUP(CommonSimpleFs) {}
TEST_TEAR_DOWN(CommonSimpleFs) {}

TEST(CommonSimpleFs, DeleteFile)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

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

TEST(CommonSimpleFs, GetMaxFileSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 0, secondFileData, secondFileDataSize);

   size_t maxFileSize = SimpleFsSDeviceGetMaxFileSize(handle, 0);

   TEST_ASSERT_EQUAL(secondFileDataSize, maxFileSize);
}

TEST(CommonSimpleFs, DeleteSomeFileVersion)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

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

   SimpleFsSDeviceDeleteFile(handle, 0);

   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, fileNewVersionData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFile);
}

TEST(CommonSimpleFs, DeleteFileWithShortageMemory)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(72, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
            SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   char secondReadFileData[secondFileDataSize];

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFirstFile);
   TEST_ASSERT_EQUAL(secondFileDataSize, sizeOfReadSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(CommonSimpleFs, FormatMemory)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   SimpleFsSDeviceFormatMemory(handle);

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFirstFile);
   TEST_ASSERT_EQUAL(0, sizeOfReadSecondFile);
}

TEST(CommonSimpleFs, ForceHistoryDeletion)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

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

   SimpleFsSDeviceForceHistoryDeletion(handle);

   char readFileData[fileNewVersionDataSize];
   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, readFileData, fileNewVersionDataSize);

   TEST_ASSERT_EQUAL(fileNewVersionDataSize, sizeOfReadFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileNewVersionData, readFileData, fileNewVersionDataSize);
}

TEST_GROUP_RUNNER(CommonSimpleFs)
{
   RUN_TEST_CASE(CommonSimpleFs, DeleteFile);
   RUN_TEST_CASE(CommonSimpleFs, GetMaxFileSize);
   RUN_TEST_CASE(CommonSimpleFs, DeleteSomeFileVersion);
   RUN_TEST_CASE(CommonSimpleFs, DeleteFileWithShortageMemory);
   RUN_TEST_CASE(CommonSimpleFs, FormatMemory);
   RUN_TEST_CASE(CommonSimpleFs, ForceHistoryDeletion);
}
