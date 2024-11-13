#include "errors.h"

#include "SDeviceCore/errors.h"
#include "SDeviceCore/heap.h"

#include "unity.h"
#include <stdlib.h>

#define ENUM_TO_STRING(enum) #enum

static bool ProcessAssertFailMustBeCalled;
static bool ProcessUnhandledThrowMustBeCalled;
static SDEVICE_HANDLE(SimpleFs) *AssertFailhandle;
static bool AreLogsShowing = true;

void ShowLog(bool value)
{
   AreLogsShowing = value;
}

void AssertionMustBeFail(bool value)
{
   ProcessAssertFailMustBeCalled = value;
}

void PanicMustBeThrown(bool value)
{
   ProcessUnhandledThrowMustBeCalled = value;
}

void SetAssertFailHandle(SDEVICE_HANDLE(SimpleFs) *handle)
{
   AssertFailhandle = handle;
}

void SDeviceProcessAssertFail(char *file, int line)
{
   if(AssertFailhandle != NULL)
         SDeviceFreeMemory(AssertFailhandle);

   if(ProcessAssertFailMustBeCalled)
   {
      char str[200];
      sprintf(str, "Test pass, assert called on file (%s) line (%d)", file, line);
      TEST_PASS_MESSAGE(str);
   }
   else
   {
      char str[200];
      sprintf(str, "Test fail, assert called on file (%s) line (%d)", file,  line);
      TEST_FAIL_MESSAGE(str);
   }
}

void SDeviceProcessPanic(const void *_handle)
{
   if(AssertFailhandle != NULL)
         SDeviceFreeMemory(AssertFailhandle);

   SDeviceHandleHeader *header = (SDeviceHandleHeader *)(_handle);
   SimpleFsSDevicePanic panic = header->LatestStatus;

   switch (panic)
   {
      case SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY:

         if(ProcessUnhandledThrowMustBeCalled)
            TEST_PASS_MESSAGE("Test pass, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY));
         else
            TEST_FAIL_MESSAGE("Test fail, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY));
         break;

      case SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW:

         if(ProcessUnhandledThrowMustBeCalled)
            TEST_PASS_MESSAGE("Test pass, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW));
         else
            TEST_FAIL_MESSAGE("Test fail, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW));
         break;
   }
}

void SDeviceProcessLogStatus(const void *handle, const void *extras, size_t extrasSize)
{
   if(!AreLogsShowing)
      return;

   SDeviceHandleHeader *header = (SDeviceHandleHeader *)(handle);
   SimpleFsSDeviceStatus status = header->LatestStatus;

   switch (status)
   {
      case SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED:

         TEST_MESSAGE(ENUM_TO_STRING(SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED));
         break;

      case SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED:
         TEST_MESSAGE(ENUM_TO_STRING(SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED));
         break;

      case SIMPLE_FS_SDEVICE_STATUS_OK:
         TEST_MESSAGE(ENUM_TO_STRING(SIMPLE_FS_SDEVICE_STATUS_OK));

      default:
         break;
   }
}
