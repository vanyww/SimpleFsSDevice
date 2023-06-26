#include "../Application/application.h"
#include "../TestDataBuilder/test_data.h"
#include "../Weak/weak.h"

#include "unity_fixture.h"


TEST_GROUP(ReadWrite);

TEST_SETUP(ReadWrite) {}
TEST_TEAR_DOWN(ReadWrite) {}

TEST(ReadWrite, WriteFileWithSizeMultipleOfFileDataBlockDataSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   uint16_t fileImageDataSize = GetTestFileImageDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileData[fileDataSize];
   char fileImageData[fileImageDataSize];
   CopyTestFileData(fileData, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   CopyTestFileImageData(fileImageData, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(64, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MEMORY_SECTORS(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(ReadWrite, WriteFileWithSizeNotMultipleOfFileDataBlockDataSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   uint16_t fileImageDataSize = GetTestFileImageDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileData[fileDataSize];
   char fileImageData[fileImageDataSize];
   CopyTestFileData(fileData, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   CopyTestFileImageData(fileImageData, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(64, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 1, fileData, fileDataSize);

   char readData[fileImageDataSize];
   memcpy(readData, &MEMORY_SECTORS(this)[1][2*sizeof(Block)], fileImageDataSize);

   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileImageData, readData, fileImageDataSize);
}

TEST(ReadWrite, ReadFileWithSizeNotMultipleOfFileDataBlockDataSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(64, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST(ReadWrite, ReadFileWithSizeMultipleOfFileDataBlockDataSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileDataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileData[fileDataSize];
   CopyTestFileData(fileData, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(64, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileData, fileDataSize);

   char readData[fileDataSize];
   SimpleFsSDeviceReadFile(handle, 0, readData, fileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(fileData, readData, fileDataSize);
}

TEST_GROUP_RUNNER(ReadWrite)
{
   RUN_TEST_CASE(ReadWrite, WriteFileWithSizeMultipleOfFileDataBlockDataSize);
   RUN_TEST_CASE(ReadWrite, WriteFileWithSizeNotMultipleOfFileDataBlockDataSize);
   RUN_TEST_CASE(ReadWrite, ReadWithSizeNotMultipleOfFileDataBlockDataSize);
   RUN_TEST_CASE(ReadWrite, ReadWithSizeMultipleOfFileDataBlockDataSize);
}
