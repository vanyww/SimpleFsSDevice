#include "../../Mock/Data/HeaderBlock/block.h"
#include "../../Mock/Bindings/sdevice_core.h"
#include "../../Mock/SDevice/simple_fs.h"
#include "../../Mock/Data/File/file.h"

#include "../../../Src/private.h"

#include "unity_fixture.h"

static void TestAssertHandler(void)
{
   ResetAssertFailHandler();

   TEST_PASS_MESSAGE("OK");
};

TEST_GROUP(Initialization);

TEST_SETUP(Initialization) { }
TEST_TEAR_DOWN(Initialization) { }

TEST(Initialization, SimpleFs)
{
   /* min size = 48 byte */
   CREATE_SIMPLE_FS_INIT(this, 48);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   TEST_ASSERT_NOT_NULL(handle);
}

TEST(Initialization, InsufficientMemoryInSector)
{
   /* min size = 48 byte */
   CREATE_SIMPLE_FS_INIT(this, 1);

   SetAssertFailHandler(TestAssertHandler);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);
}

TEST(Initialization, S0_STATE_ACTIVE_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ACTIVE_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ACTIVE_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_AUXILIARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ACTIVE_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_AUXILIARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_END_S1STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_END_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_END_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_TRANSFER_END_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ERASED_S1_STATE_TRANSFER_ONGOING)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_ONGOING);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_PRIMARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ERASED_S1_STATE_ERASED)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_AUXILIARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ERASED_S1_STATE_TRANSFER_END)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_TRANSFER_END);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_AUXILIARY_SECTOR(this)));
}

TEST(Initialization, S0_STATE_ERASED_S1_STATE_ACTIVE)
{
   CREATE_SIMPLE_FS_INIT(this, 48);

   Block primarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ERASED);
   Block auxiliarySectorHeader = MockCreateHeaderBlock(SECTOR_STATE_ACTIVE);
   MockWriteUInt64(NULL, &SIMPLE_FS_PRIMARY_SECTOR(this), 8, primarySectorHeader.AsValue);
   MockWriteUInt64(NULL, &SIMPLE_FS_AUXILIARY_SECTOR(this), 8, auxiliarySectorHeader.AsValue);

   SIMPLE_FS_DISPOSE_HANDLE_CLEANUP_ATTRIBUTE SDEVICE_HANDLE(SimpleFs) *handle =
         SDEVICE_CREATE_HANDLE(SimpleFs)(&SIMPLE_FS_INIT(this), NULL);

   WriteStream *stream = handle->Runtime->ActiveWriteStream;

   TEST_ASSERT(IsSimpleFsSectorsEquial(stream->Sector, &SIMPLE_FS_AUXILIARY_SECTOR(this)));
}

TEST_GROUP_RUNNER(Initialization)
{
   RUN_TEST_CASE(Initialization, SimpleFs);
   RUN_TEST_CASE(Initialization, InsufficientMemoryInSector);
   RUN_TEST_CASE(Initialization, S0_STATE_ACTIVE_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(Initialization, S0_STATE_ACTIVE_S1_STATE_ERASED);
   RUN_TEST_CASE(Initialization, S0_STATE_ACTIVE_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(Initialization, S0_STATE_ACTIVE_S1_STATE_ACTIVE);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_ERASED);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_ONGOING_S1_STATE_ACTIVE);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_END_S1STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_END_S1_STATE_ERASED);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_END_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(Initialization, S0_STATE_TRANSFER_END_S1_STATE_ACTIVE);
   RUN_TEST_CASE(Initialization, S0_STATE_ERASED_S1_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(Initialization, S0_STATE_ERASED_S1_STATE_ERASED);
   RUN_TEST_CASE(Initialization, S0_STATE_ERASED_S1_STATE_TRANSFER_END);
   RUN_TEST_CASE(Initialization, S0_STATE_ERASED_S1_STATE_ACTIVE);
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
