#include "../Application/application.h"
#include "../Weak/weak.h"

#include "unity_fixture.h"


TEST_GROUP(InitializationTests);

TEST_SETUP(InitializationTests) {}

TEST_TEAR_DOWN(InitializationTests)
{
   AssertFailhandle = NULL;
}


TEST(InitializationTests, HandleInitialization)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64); // min size = 64 byte

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
}


TEST(InitializationTests, InsufficientMemoryInSector)
{
   ProcessAssertFailMustBeCalled = true;

   CREATE_SIMPLE_FS_APPLICATION(1); // min size = 64 byte

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
}


TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_ONGOING)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ERASED)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_END)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
}


TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ACTIVE)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_ONGOING)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

  Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ERASED)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_END)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ACTIVE)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
}


TEST(InitializationTests, S0_STATE_TRANSFER_END_S1STATE_TRANSFER_ONGOING)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ERASED)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_TRANSFER_END)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ACTIVE)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_ONGOING)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
}


TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_ERASED)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
}


TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_END)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
}


TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_ACTIVE)
{
   ProcessAssertFailMustBeCalled = false;

   CREATE_SIMPLE_FS_APPLICATION(64);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &sector$0, 8, headerS0.AsValue);
   WriteUInt64(NULL, &sector$1, 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

   WriteStream *stream = handle->Runtime.ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
}

TEST_GROUP_RUNNER(InitializationTests)
{
   RUN_TEST_CASE(InitializationTests, HandleInitialization);
   RUN_TEST_CASE(InitializationTests, InsufficientMemoryInSector);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ERASED);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ACTIVE);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ERASED);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ACTIVE);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_END_S1STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ERASED);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ACTIVE);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ERASED_S1_STATE_ERASED);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ERASED_S1_STATE_ACTIVE);
}


/* Sector state table during initialization */
/*
 * S0 - SECTOR_STATE_ACTIVE:
 *    S1 - SECTOR_STATE_TRANSFER_ONGOING ------- [FormatStreamSector(S1),SetActiveWriteStream(S0)]
 *    S1 - SECTOR_STATE_ERASED ----------------- [SetActiveWriteStream(S0)]
 *    S1 - SECTOR_STATE_TRANSFER_END ----------- [FormatStreamSector(S0), SetActiveWriteStream(S1)]
 *    S1 - SECTOR_STATE_ACTIVE ----------------- [ForceReformat]
 *
 * S0 - SECTOR_STATE_TRANSFER_ONGOING:
 *    S1 - SECTOR_STATE_TRANSFER_ONGOING ------- [ForceReformat]
 *    S1 - SECTOR_STATE_ERASED ----------------- [ForceReformat]
 *    S1 - SECTOR_STATE_TRANSFER_END ----------- [ForceReformat]
 *    S1 - SECTOR_STATE_ACTIVE ----------------- [FormatStreamSector(S0),SetActiveWriteStream(S1)]
 *
 * S0 - SECTOR_STATE_TRANSFER_END:
 *    S1 - SECTOR_STATE_TRANSFER_ONGOING ------- [ForceReformat]
 *    S1 - SECTOR_STATE_ERASED ----------------- [SetActiveWriteStream(S0)]
 *    S1 - SECTOR_STATE_TRANSFER_END ----------- [ForceReformat]
 *    S1 - SECTOR_STATE_ACTIVE------------------ [FormatStreamSector(S1),SetActiveWriteStream(S0)]
 *
 * S0 - SECTOR_STATE_ERASED:
 *    S1 - SECTOR_STATE_TRANSFER_ONGOING ------- [ForceReformat]
 *    S1 - SECTOR_STATE_ERASED ----------------- [SetActiveWriteStream(S1)]
 *    S1 - SECTOR_STATE_TRANSFER_END ----------- [SetActiveWriteStream(S1)]
 *    S1 - SECTOR_STATE_ACTIVE ----------------- [SetActiveWriteStream(S1)]
 *
 * */
