#include <stdlib.h>
#include <time.h>
#include "unity_fixture.h"

#include "../../Mock/Error/errors.h"
#include "../../Mock/SimpleFs/Data/test_data_builder.h"
#include "../../Mock/SimpleFs/Application/application.h"

TEST_GROUP(Transfer);

TEST_SETUP(Transfer)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);
}

TEST_TEAR_DOWN(Transfer) {}

TEST(Transfer, TransferTwoFiles)
{
   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(sizeOfFirstFile, firstFileDataSize);
   TEST_ASSERT_EQUAL(sizeOfSecondFile, secondFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFile)
{
   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      if(i % 2)
         SimpleFsSDeviceWriteFile(handle, 0, secondFileData, secondFileDataSize);
      else
         SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char secondReadFileData[secondFileDataSize];

   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 0, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(sizeOfReadSecondFile, secondFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithBadBlocks)
{
   ShowLog(false);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileImageData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileImageData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   uint16_t badBlockNumbers[] = {2, 6, 10, 14, 18, 22};
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 0);
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 1);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFileAndBadBlocks)
{
   ShowLog(false);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   uint16_t badBlockNumbers[] = {2, 6, 10, 14, 18, 22};
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 0);
   SetTestBadBlocksConfig(badBlockNumbers, sizeof(badBlockNumbers), 1);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFileAndRandomGenerationBadBlocks)
{
   ShowLog(false);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_FULLY_FILLED_DATA_BLOCKS);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_INCOMPLETE_FILLED_LAST_DATA_BLOCK);

   uint16_t amountOfBadBlocks = 7;
   uint16_t amountBlocksInSector = 22;
   uint16_t badBlockNumbersSector$1[amountOfBadBlocks];
   uint16_t badBlockNumbersSector$2[amountOfBadBlocks];

   srand(time(NULL));
   for (uint16_t i = 0; i < amountOfBadBlocks; ++i)
      badBlockNumbersSector$1[i] = 2 + rand()%amountBlocksInSector;

   for (uint16_t i = 0; i < amountOfBadBlocks; ++i)
      badBlockNumbersSector$2[i] = 2 + rand()%amountBlocksInSector;

   SetTestBadBlocksConfig(badBlockNumbersSector$1, sizeof(badBlockNumbersSector$1), 0);
   SetTestBadBlocksConfig(badBlockNumbersSector$2, sizeof(badBlockNumbersSector$2), 1);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      for (uint16_t j = 0; j < amountOfBadBlocks; ++j)
         badBlockNumbersSector$1[j] = 2 + rand()%amountBlocksInSector;

      for (uint16_t j = 0; j < amountOfBadBlocks; ++j)
         badBlockNumbersSector$1[j] = 2 + rand()%amountBlocksInSector;

      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST_GROUP_RUNNER(Transfer)
{
   RUN_TEST_CASE(Transfer, TransferTwoFiles);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFile);
   RUN_TEST_CASE(Transfer, TransferWithBadBlocks);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFileAndBadBlocks);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFileAndRandomGenerationBadBlocks);
}
