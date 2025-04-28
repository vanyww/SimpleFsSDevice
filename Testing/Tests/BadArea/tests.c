#include "../../Mock/Bindings/sdevice_core.h"
#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "unity_fixture.h"

#include "SDeviceCore/common.h"

static const void *PanicHandle = NULL;
static SDevicePanic Panic = UINT16_MAX;

static void TestPanicHandler(const void *handle, SDevicePanic panic)
{
   PanicHandle = handle;
   Panic = panic;

   TEST_MESSAGE("OK");
}

TEST_GROUP(BadArea);

TEST_SETUP(BadArea) { }
TEST_TEAR_DOWN(BadArea)
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

TEST(BadArea, EveryFourthBlockIsBad)
{
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   size_t badBlockIdxs[] = {0, 4, 8, 12, 16, 20, 24};

   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = badBlockIdxs,
      .AuxiliarySectorBadBlocksIdxs      = badBlockIdxs,
      .PrimarySectorBadBlocksIdxsCount   = LENGTHOF(badBlockIdxs),
      .AuxiliarySectorBadBlocksIdxsCount = LENGTHOF(badBlockIdxs)
   };

   SetMockBadBlocks(badBlocks);

   CREATE_SIMPLE_FS_INIT(this, 200);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

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
   size_t firstFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS);
   char firstFileData[firstFileDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_FULLY_FILLED_DATA_BLOCKS, firstFileData);

   size_t secondFileDataSize = MockGetFileSize(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK);
   char secondFileData[secondFileDataSize];
   MockGetFileImageData(MOCK_FILE_TYPE_INCOMPLETE_FILLED_LAST_DATA_BLOCK, secondFileData);

   size_t badBlockIdxs[] = {2, 6, 10, 14, 16, 18, 20, 22, 24};

   MockBadBlocks badBlocks =
   {
      .PrimarySectorBadBlocksIdxs        = badBlockIdxs,
      .AuxiliarySectorBadBlocksIdxs      = badBlockIdxs,
      .PrimarySectorBadBlocksIdxsCount   = LENGTHOF(badBlockIdxs),
      .AuxiliarySectorBadBlocksIdxsCount = LENGTHOF(badBlockIdxs)
   };

   SetMockBadBlocks(badBlocks);

   CREATE_SIMPLE_FS_INIT(this, 200);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   SimpleFsSDeviceWriteFile(handle, 0, firstFileData, firstFileDataSize);

   char firstReadFileData[firstFileDataSize];
   size_t sizeOfFirstFile = SimpleFsSDeviceReadFile(handle, 0, firstReadFileData, firstFileDataSize);

   TEST_ASSERT_EQUAL(firstFileDataSize, sizeOfFirstFile);
   TEST_ASSERT_EQUAL_CHAR_ARRAY(firstFileData, firstReadFileData, firstFileDataSize);

   SetPanicHandler(TestPanicHandler);

   SimpleFsSDeviceWriteFile(handle, 1, secondFileData, secondFileDataSize);

   ResetPanicHandler();

   TEST_ASSERT_EQUAL(handle, PanicHandle);
   TEST_ASSERT_EQUAL_INT16(SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY, Panic);
}

TEST_GROUP_RUNNER(BadArea)
{
   RUN_TEST_CASE(BadArea, EveryFourthBlockIsBad);
   RUN_TEST_CASE(BadArea, WriteFileWhenShortageMemoryDueBadBlocks);
}
