#include "../../Mock/Errors/errors.h"
#include "../../Mock/SimpleFs/Data/HeaderBlock/block.h"
#include "../../Mock/SimpleFs/Application/application.h"

#include "../../../Src/private.h"

#include "unity_fixture.h"

TEST_GROUP(InitializationTests);

TEST_SETUP(InitializationTests)
{
   AssertionMustBeFail(false);
   PanicMustBeThrown(false);
}

TEST_TEAR_DOWN(InitializationTests) { }

TEST(InitializationTests, HandleInitialization)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this); // min size = 48 byte

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);
}

TEST(InitializationTests, InsufficientMemoryInSector)
{
   AssertionMustBeFail(true);
   SetAssertFailHandle(NULL);

   CREATE_SIMPLE_FS_APPLICATION(1, this); // min size = 48 byte

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);

   TEST_FAIL_MESSAGE("Test fail, assert was not called");
}

TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$1(this)));
}

TEST(InitializationTests, S0_STATE_ACTIVE_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

  Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$1(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_END_S1STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_TRANSFER_END_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$0(this)));
}

TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$1(this)));
}

TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_TRANSFER_END, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$1(this)));
}

TEST(InitializationTests, S0_STATE_ERASED_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_APPLICATION(48, this);

   Block headerS0 = CreateHeaderBlock(SECTOR_STATE_ERASED, true);
   Block headerS1 = CreateHeaderBlock(SECTOR_STATE_ACTIVE, true);
   WriteUInt64(NULL, &SECTOR$0(this), 8, headerS0.AsValue);
   WriteUInt64(NULL, &SECTOR$1(this), 8, headerS1.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&INIT(this), NULL, 0, NULL);
   SetAssertFailHandle(handle);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;
   TEST_ASSERT(IsSectorEquial(stream->Sector, &SECTOR$1(this)));
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
