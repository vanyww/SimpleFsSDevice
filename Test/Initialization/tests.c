#include "../Application/application.h"
#include "../Weak/weak.h"
#include "../Inc/SimpleFsSDevice/public.h"
#include "../Src/private.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/Enumerations/sector_state.h"
#include "../Src/Mid-layer/sector_state.h"
#include "../Src/Mid-layer/Common/stream_helpers.h"

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

   _createApplication(64); // min size = 64 byte
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);

   _cleanup SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

}

TEST(InitializationTests, InsufficientMemoryInSector)
{
   ProcessAssertFailMustBeCalled = true;

   _createApplication(1); // min size = 64 byte
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);


   _cleanup SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

}

TEST(InitializationTests, S0_STATE_ACTIVE)
{
   ProcessAssertFailMustBeCalled = false;

   _createApplication(64);
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);

   /* create a handle to write the desired status */
   SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$0WriteStream, SECTOR_STATE_ACTIVE);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle);
   /* result: S0 - active, S1 - transfer ongoing */


   /* S0 - SECTOR_STATE_ACTIVE: S1 - SECTOR_STATE_TRANSFER_ONGOING */
   SDEVICE_HANDLE(SimpleFs) *handle1 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStream *stream = GetActiveWriteStream(handle1);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle1);
   /* result: S0 - active, S1 - erased */


   /* S0 - SECTOR_STATE_ACTIVE: S1 - SECTOR_STATE_ERASED */
   SDEVICE_HANDLE(SimpleFs) *handle2 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle2);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_END);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle2);
   /* result: S0 - active, S1 - transfer end */


   /* S0 - SECTOR_STATE_ACTIVE: S1 - SECTOR_STATE_TRANSFER_END */
   SDEVICE_HANDLE(SimpleFs) *handle3 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle3);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$0WriteStream, SECTOR_STATE_ACTIVE);
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle3);
   /* result: S0 - active, S1 - active */


   /* S0 - SECTOR_STATE_ACTIVE: S1 - SECTOR_STATE_ACTIVE */
   SDEVICE_HANDLE(SimpleFs) *handle4 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle4);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle4);
}

TEST(InitializationTests, S0_STATE_TRANSFER_ONGOING)
{
   ProcessAssertFailMustBeCalled = false;

   _createApplication(64);
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);

   /* create a handle to write the desired status */
   SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle);
   /* result: S0 - transfer ongoing, S1 - transfer ongoing */


   /* S0 - SECTOR_STATE_TRANSFER_ONGOING: S1 - SECTOR_STATE_TRANSFER_ONGOING */
   SDEVICE_HANDLE(SimpleFs) *handle1 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStream *stream = GetActiveWriteStream(handle1);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$1WriteStream, SECTOR_STATE_ERASED);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle1);
   /* result: S0 - transfer ongoing, S1 - erased */


   /* S0 - SECTOR_STATE_TRANSFER_ONGOING: S1 - SECTOR_STATE_ERASED */
   SDEVICE_HANDLE(SimpleFs) *handle2 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle2);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_END);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle2);
   /* result: S0 - transfer ongoing, S1 - transfer end */


   /* S0 - SECTOR_STATE_TRANSFER_ONGOING: S1 - SECTOR_STATE_TRANSFER_END */
   SDEVICE_HANDLE(SimpleFs) *handle3 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle3);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle3);
   /* result: S0 - transfer ongoing, S1 - active */


   /* S0 - SECTOR_STATE_TRANSFER_ONGOING: S1 - SECTOR_STATE_ACTIVE */
   SDEVICE_HANDLE(SimpleFs) *handle4 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle4);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle4);
}


TEST(InitializationTests, S0_STATE_TRANSFER_END)
{
   ProcessAssertFailMustBeCalled = false;

   _createApplication(64);
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);


   /* create a handle to write the desired status */
   SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_END);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle);
   /* result: S0 - transfer end, S1 - transfer ongoing */


   /* S0 - SECTOR_STATE_TRANSFER_END: S1 - SECTOR_STATE_TRANSFER_ONGOING */
   SDEVICE_HANDLE(SimpleFs) *handle1 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStream *stream = GetActiveWriteStream(handle1);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_END);
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$1WriteStream, SECTOR_STATE_ERASED);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle1);
   /* result: S0 - transfer end, S1 - erased */


   /* S0 - SECTOR_STATE_TRANSFER_END: S1 - SECTOR_STATE_ERASED */
   SDEVICE_HANDLE(SimpleFs) *handle2 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle2);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_END);
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_END);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle2);
   /* result: S0 - transfer end, S1 - transfer end */


   /* S0 - SECTOR_STATE_TRANSFER_END: S1 - SECTOR_STATE_TRANSFER_END */
   SDEVICE_HANDLE(SimpleFs) *handle3 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle3);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$0WriteStream, SECTOR_STATE_TRANSFER_END);
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle3);
   /* result: S0 - transfer end, S1 - active */


   /* S0 - SECTOR_STATE_TRANSFER_END: S1 - SECTOR_STATE_ACTIVE */
   SDEVICE_HANDLE(SimpleFs) *handle4 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle4);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle4);
}

TEST(InitializationTests, S0_STATE_ERASED)
{
   ProcessAssertFailMustBeCalled = false;

   _createApplication(64);
   memset(&memorySectors[0][0],0, SectorSize);
   memset(&memorySectors[1][0],0, SectorSize);


   /* create a handle to write the desired status */
   SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$0WriteStream, SECTOR_STATE_ERASED);
   WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_ONGOING);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle);
   /* result: S0 - erased, S1 - transfer ongoing */


   /* S0 - SECTOR_STATE_ERASED: S1 - SECTOR_STATE_TRANSFER_ONGOING */
   SDEVICE_HANDLE(SimpleFs) *handle1 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   WriteStream *stream = GetActiveWriteStream(handle1);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$0));
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$0WriteStream, SECTOR_STATE_ERASED);
   WriteStreamSectorState(handle1, &handle1->Runtime.Sector$1WriteStream, SECTOR_STATE_ERASED);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle1);
   /* result: S0 - erased, S1 - erased */


   /* S0 - SECTOR_STATE_ERASED: S1 - SECTOR_STATE_ERASED */
   SDEVICE_HANDLE(SimpleFs) *handle2 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle2);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$0WriteStream, SECTOR_STATE_ERASED);
   WriteStreamSectorState(handle2, &handle2->Runtime.Sector$1WriteStream, SECTOR_STATE_TRANSFER_END);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle2);
   /* result: S0 - erased, S1 - transfer end */


   /* S0 - SECTOR_STATE_ERASED: S1 - SECTOR_STATE_TRANSFER_END */
   SDEVICE_HANDLE(SimpleFs) *handle3 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle3);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$0WriteStream, SECTOR_STATE_ERASED);
   WriteStreamSectorState(handle3, &handle3->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle3);
   /* result: S0 - erased, S1 - active */


   /* S0 - SECTOR_STATE_ERASED: S1 - SECTOR_STATE_ACTIVE */
   SDEVICE_HANDLE(SimpleFs) *handle4 = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);
   stream = GetActiveWriteStream(handle4);
   TEST_ASSERT(IsSectorEquial(stream->Sector, &sector$1));
   SDEVICE_DISPOSE_HANDLE(SimpleFs)(&handle4);
}

TEST_GROUP_RUNNER(InitializationTests)
{
   RUN_TEST_CASE(InitializationTests, HandleInitialization);
   RUN_TEST_CASE(InitializationTests, InsufficientMemoryInSector);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ACTIVE);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_ONGOING);
   RUN_TEST_CASE(InitializationTests, S0_STATE_TRANSFER_END);
   RUN_TEST_CASE(InitializationTests, S0_STATE_ERASED);
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
