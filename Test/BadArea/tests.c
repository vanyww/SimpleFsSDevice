#include "unity_fixture.h"

#include "../Application/application.h"
#include "../TestDataBuilder/test_data.h"
#include "../Weak/weak.h"

#include "../Src/IO/Primitives/Block/bad_area_tag_block.h"

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
   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileImageData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   uint16_t badBlockNumbers[] = {0, 4, 8, 12, 16, 20, 24};
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, 14);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, 17);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
}

TEST(BadArea, EverySecondBlockIsBad)
{
   SetProcessUnhandledThrowMustBeCalled(true);

   uint16_t badBlockNumbers[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   TEST_FAIL_MESSAGE("Test fail, no exception was thrown");
}

TEST(BadArea, WriteFileWhenShortageMemoryDueBadBlocks)
{
   SetProcessUnhandledThrowMustBeCalled(true);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileImageData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   uint16_t badBlockNumbers[] = {2, 6, 10, 14, 16, 18, 20, 22, 24};
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, 14);
   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);

   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
   TEST_FAIL_MESSAGE("Test fail, no exception was thrown");
}

TEST_GROUP_RUNNER(BadArea)
{
   RUN_TEST_CASE(BadArea, EveryFourthBlockIsBad);
   RUN_TEST_CASE(BadArea, EverySecondBlockIsBad);
   RUN_TEST_CASE(BadArea, WriteFileWhenShortageMemoryDueBadBlocks);
}
