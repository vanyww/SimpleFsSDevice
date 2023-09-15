#include "../../Mock/Error/errors.h"
#include "../../Mock/SimpleFs/Data/BadBlock/block.h"
#include "../../Mock/SimpleFs/Application/application.h"
#include "../../Mock/SimpleFs/Data/TestDataBuilder/data_builder.h"

#include "unity_fixture.h"

TEST_GROUP(BadArea);

TEST_SETUP(BadArea)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);
   ShowLog(false);
}
TEST_TEAR_DOWN(BadArea) {}

TEST(BadArea, EveryFourthBlockIsBad)
{
   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileImageData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   size_t badBlockNumbers[] = {0, 4, 8, 12, 16, 20, 24};
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 0);
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 1);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(secondFileDataSize, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(BadArea, WriteFileWhenShortageMemoryDueBadBlocks)
{
   SetProcessUnhandledThrowMustBeCalled(true);

   size_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   size_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileImageData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   size_t badBlockNumbers[] = {2, 6, 10, 14, 16, 18, 20, 22, 24};
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 0);
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 1);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   char firstReadFileData[firstFileDataSize];
   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);

   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
   TEST_FAIL_MESSAGE("Test fail, no exception was thrown");
}

TEST_GROUP_RUNNER(BadArea)
{
   RUN_TEST_CASE(BadArea, EveryFourthBlockIsBad);
   RUN_TEST_CASE(BadArea, WriteFileWhenShortageMemoryDueBadBlocks);
}
