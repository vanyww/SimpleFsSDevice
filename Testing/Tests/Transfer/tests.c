#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "unity_fixture.h"

#include "SDeviceCore/common.h"

#include <stdlib.h>
#include <time.h>

TEST_GROUP(Transfer);

TEST_SETUP(Transfer) { }

TEST_TEAR_DOWN(Transfer)
{
   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = NULL,
      .AuxiliarySectorBadBlocksIdxs      = NULL,
      .PrimarySectorBadBlocksIdxsCount   = 0,
      .AuxiliarySectorBadBlocksIdxsCount = 0
   };

   SetMockBadBlocks(badBlocks);
}

TEST(Transfer, TransferTwoFiles)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (size_t i = 0; i < 10000; i++)
      SimpleFsSDeviceForceHistoryDeletion(handle);

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(sizeOfFirstFile, firstFileDataSize);
   TEST_ASSERT_EQUAL_UINT(sizeOfSecondFile, secondFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFile)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 100);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

   for (size_t i = 0; i < 10000; i++)
   {
      (i % 2)                                                                 ?
      SimpleFsSDeviceWriteFile(handle, 0, secondFileData, secondFileDataSize) :
      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char secondReadFileData[secondFileDataSize];

   size_t sizeOfReadSecondFile = SimpleFsSDeviceReadFile(handle, 0, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(sizeOfReadSecondFile, secondFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithBadBlocks)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 200);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   size_t badBlockIdxs[] = {2, 6, 10, 14, 18, 22};

   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = badBlockIdxs,
      .AuxiliarySectorBadBlocksIdxs      = badBlockIdxs,
      .PrimarySectorBadBlocksIdxsCount   = LENGTHOF(badBlockIdxs),
      .AuxiliarySectorBadBlocksIdxsCount = LENGTHOF(badBlockIdxs)
   };

   SetMockBadBlocks(badBlocks);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (size_t i = 0; i < 10000; i++)
      SimpleFsSDeviceForceHistoryDeletion(handle);

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL_UINT(secondFileDataSize, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFileAndBadBlocks)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 200);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   size_t badBlockIdxs[] = {2, 6, 10, 14, 18, 22};

   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = badBlockIdxs,
      .AuxiliarySectorBadBlocksIdxs      = badBlockIdxs,
      .PrimarySectorBadBlocksIdxsCount   = LENGTHOF(badBlockIdxs),
      .AuxiliarySectorBadBlocksIdxsCount = LENGTHOF(badBlockIdxs)
   };

   SetMockBadBlocks(badBlocks);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (size_t i = 0; i < 10000; i++)
   {
      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL(secondFileDataSize, sizeOfSecondFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(secondFileData, secondReadFileData, secondFileDataSize);
}

TEST(Transfer, TransferWithRewriteFileAndRandomGenerationBadBlocks)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   CREATE_SIMPLE_FS_INIT(this, 200);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   size_t BadBlocksCount = 7;
   size_t BlocksInSectorCount = 22;
   size_t PrimarySectorbadBlockIdxs[BadBlocksCount];
   size_t auxiliarySectorBadBlocksIdxs[BadBlocksCount];

   srand(time(NULL));

   for (size_t i = 0; i < BadBlocksCount; ++i)
      PrimarySectorbadBlockIdxs[i] = 2 + rand()%BlocksInSectorCount;

   for (size_t i = 0; i < BadBlocksCount; ++i)
      auxiliarySectorBadBlocksIdxs[i] = 2 + rand()%BlocksInSectorCount;

   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = PrimarySectorbadBlockIdxs,
      .AuxiliarySectorBadBlocksIdxs      = auxiliarySectorBadBlocksIdxs,
      .PrimarySectorBadBlocksIdxsCount   = LENGTHOF(PrimarySectorbadBlockIdxs),
      .AuxiliarySectorBadBlocksIdxsCount = LENGTHOF(auxiliarySectorBadBlocksIdxs)
   };

   SetMockBadBlocks(badBlocks);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   for (size_t i = 0; i < 10000; i++)
   {
      for (size_t j = 0; j < BadBlocksCount; ++j)
         PrimarySectorbadBlockIdxs[j] = 2 + rand()%BlocksInSectorCount;

      for (size_t j = 0; j < BadBlocksCount; ++j)
         PrimarySectorbadBlockIdxs[j] = 2 + rand()%BlocksInSectorCount;

      SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);
      SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);
      SimpleFsSDeviceForceHistoryDeletion(handle);
   }

   char firstReadFileData[firstFileDataSize];
   char secondReadFileData[secondFileDataSize];

   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);
   size_t sizeOfSecondFile = SimpleFsSDeviceReadFile(handle, 1, secondReadFileData, secondFileDataSize);

   TEST_ASSERT_EQUAL_UINT(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL_UINT(secondFileDataSize, sizeOfSecondFile);
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
