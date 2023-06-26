#include "unity_fixture.h"

#include "../Application/application.h"
#include "../TestDataBuilder/test_data.h"
#include "../Weak/weak.h"


TEST_GROUP(CommonSimpleFs);

TEST_SETUP(CommonSimpleFs) {}
TEST_TEAR_DOWN(CommonSimpleFs) {}

TEST(CommonSimpleFs, DeleteFile)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t file0DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file0Data[file0DataSize];
   CopyTestFileData(file0Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t file1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file1Data[file1DataSize];
   CopyTestFileData(file1Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, file0Data, file0DataSize);
   SimpleFsSDeviceWriteFile(handle, 1, file1Data, file1DataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   size_t sizeOfReadFile0;
   size_t sizeOfReadFile1;

   sizeOfReadFile0 = SimpleFsSDeviceReadFile(handle, 0, file0Data, file0DataSize);
   sizeOfReadFile1 = SimpleFsSDeviceReadFile(handle, 1, file1Data, file1DataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFile0);
   TEST_ASSERT_EQUAL(file1DataSize, sizeOfReadFile1);
}

TEST(CommonSimpleFs, GetMaxFileSize)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t file0DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file0Data[file0DataSize];
   CopyTestFileData(file0Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t file1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file1Data[file1DataSize];
   CopyTestFileData(file1Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, file0Data, file0DataSize);
   SimpleFsSDeviceWriteFile(handle, 0, file1Data, file1DataSize);

   size_t maxFileSize = SimpleFsSDeviceGetMaxFileSize(handle, 0);

   TEST_ASSERT_EQUAL(file1DataSize, maxFileSize);
}

TEST(CommonSimpleFs, DeleteSomeFileVersion)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileVersion1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileVersion1Data[fileVersion1DataSize];
   CopyTestFileData(fileVersion1Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t fileVersion2DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileVersion2Data[fileVersion2DataSize];
   CopyTestFileData(fileVersion2Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileVersion1Data, fileVersion1DataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileVersion2Data, fileVersion2DataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, fileVersion2Data, fileVersion2DataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFile);
}

TEST(CommonSimpleFs, DeleteFileWithShortageMemory)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t file1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file1Data[file1DataSize];
   CopyTestFileData(file1Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t file2DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file2Data[file2DataSize];
   CopyTestFileData(file2Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(72, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
            SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, file1Data, file1DataSize);
   SimpleFsSDeviceWriteFile(handle, 1, file2Data, file2DataSize);

   SimpleFsSDeviceDeleteFile(handle, 0);

   size_t sizeOfReadFile1 = SimpleFsSDeviceReadFile(handle, 0, file1Data, file1DataSize);
   size_t sizeOfReadFile2 = SimpleFsSDeviceReadFile(handle, 1, file2Data, file2DataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFile1);
   TEST_ASSERT_EQUAL(file2DataSize, sizeOfReadFile2);
}

TEST(CommonSimpleFs, FormatMemory)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t file1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file1Data[file1DataSize];
   CopyTestFileData(file1Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t file2DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char file2Data[file2DataSize];
   CopyTestFileData(file2Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, file1Data, file1DataSize);
   SimpleFsSDeviceWriteFile(handle, 1, file2Data, file2DataSize);

   SimpleFsSDeviceFormatMemory(handle);

   size_t sizeOfReadFile1 = SimpleFsSDeviceReadFile(handle, 0, file1Data, file1DataSize);
   size_t sizeOfReadFile2 = SimpleFsSDeviceReadFile(handle, 1, file2Data, file2DataSize);

   TEST_ASSERT_EQUAL(0, sizeOfReadFile1);
   TEST_ASSERT_EQUAL(0, sizeOfReadFile2);
}

TEST(CommonSimpleFs, ForceHistoryDeletion)
{
   SetProcessAssertFailMustBeCalled(false);
   SetProcessUnhandledThrowMustBeCalled(false);

   uint16_t fileVersion1DataSize = GetTestFileDataSize(FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileVersion1Data[fileVersion1DataSize];
   CopyTestFileData(fileVersion1Data, FILE_WITH_SIZE_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   uint16_t fileVersion2DataSize = GetTestFileDataSize(FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);
   char fileVersion2Data[fileVersion2DataSize];
   CopyTestFileData(fileVersion2Data, FILE_WITH_SIZE_NOT_MULTIPLE_OF_FileDataBlock_Data_SIZE);

   CREATE_SIMPLE_FS_APPLICATION(100, this);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailhandle(handle);

   SimpleFsSDeviceWriteFile(handle, 0, fileVersion1Data, fileVersion1DataSize);
   SimpleFsSDeviceWriteFile(handle, 0, fileVersion2Data, fileVersion2DataSize);

   SimpleFsSDeviceForceHistoryDeletion(handle);

   size_t sizeOfReadFile = SimpleFsSDeviceReadFile(handle, 0, fileVersion2Data, fileVersion2DataSize);

   TEST_ASSERT_EQUAL(fileVersion2DataSize, sizeOfReadFile);
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
