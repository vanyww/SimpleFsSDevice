#include "../Application/application.h"
#include "../Weak/weak.h"
#include "../Inc/SimpleFsSDevice/public.h"

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

   _cleanup SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

}

TEST(InitializationTests, InsufficientMemoryInSector)
{
   ProcessAssertFailMustBeCalled = true;

   _createApplication(1); // min size = 64 byte

   _cleanup SDEVICE_HANDLE(SimpleFs) *handle = SDEVICE_CREATE_HANDLE(SimpleFs)(&init, NULL, 0, NULL);

}


TEST_GROUP_RUNNER(InitializationTests)
{
   RUN_TEST_CASE(InitializationTests,HandleInitialization);
   RUN_TEST_CASE(InitializationTests, InsufficientMemoryInSector);
}
