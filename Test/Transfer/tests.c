#include "unity_fixture.h"

#include "../Application/application.h"
#include "../TestDataBuilder/test_data.h"
#include "../Weak/weak.h"


TEST_GROUP(Transfer);

TEST_SETUP(Transfer) {}
TEST_TEAR_DOWN(Transfer) {}

TEST(Transfer, TransferTwoFiles)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

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
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t firstFileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char firstFileData[firstFileDataSize];
   CopyTestFileData(firstFileData, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t secondFileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char secondFileData[secondFileDataSize];
   CopyTestFileData(secondFileData, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   int temp = 1;

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

TEST_GROUP_RUNNER(Transfer)
{
   RUN_TEST_CASE(Transfer, TransferTwoFiles);
   RUN_TEST_CASE(Transfer, TransferWithRewriteFile);
}
