#include "unity_fixture.h"

#include "../Application/application.h"
#include "../TestDataBuilder/test_data.h"
#include "../Weak/weak.h"
#include <stdlib.h>
#include <time.h>


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
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   size_t sizeOfReadFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, firstFileDataSize);
   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(sizeOfReadFirstFile, firstFileDataSize);
   TEST_ASSERT_EQUAL(sizeOfReadSecondFile, secondFileDataSize);
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
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      if(i % 2)
         SimpleFsSDeviceWriteFile(handle, 0, secondFileData, secondFileDataSize);
      else
         SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 0, secondFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(sizeOfReadSecondFile, secondFileDataSize);
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
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, 14);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, 17);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
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
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
   }

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, 14);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, 17);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
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
   uint16_t amountBlocksInSector = 24;
   uint16_t badBlockNumbers[amountOfBadBlocks];

   srand(time(NULL));
   for (uint16_t i = 0; i < amountOfBadBlocks; ++i)
      badBlockNumbers[i] = rand()%amountBlocksInSector;

   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 1);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 1);
   SetGlobalBadBlocksNumbersArrayPtr(badBlockNumbers, 0);
   SetGlobalBadBlocksNumbersArraySize(sizeof(badBlockNumbers), 0);

   CREATE_SIMPLE_FS_APPLICATION(200, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (uint16_t i = 0; i < 10000; i++)
   {
      for (uint16_t j = 0; j < amountOfBadBlocks; ++j)
         badBlockNumbers[j] = rand()%amountBlocksInSector;

      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
   }

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstFileData, 14);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondFileData, 17);

   TEST_ASSERT_EQUAL(14, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(17, sizeOfSecondFile);
}

TEST_GROUP_RUNNER(Transfer)
{
   RUN_TEST_CASE(Transfer, TransferTwoFiles);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFile);
   RUN_TEST_CASE(Transfer, TransferWithBadBlocks);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFileAndBadBlocks);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFileAndRandomGenerationBadBlocks);
}
