#include "weak.h"
#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/global.h"

#include "unity.h"
#include <stdio.h>
#define ENUM_TO_STRING(enum) #enum

static bool ProcessAssertFailMustBeCalled;
static bool ProcessUnhandledThrowMustBeCalled;
static SDEVICE_HANDLE(SimpleFs) *AssertFailhandle;

void SDeviceProcessAssertFail(char *file, int line)
{
   if(AssertFailhandle != NULL)
         SDeviceFree(AssertFailhandle);

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

void SDeviceProcessUnhandledThrow(const void *_handle)
{
   if(AssertFailhandle != NULL)
         SDeviceFree(AssertFailhandle);

   SDeviceHandleHeader *header = (SDeviceHandleHeader *)(_handle);
   SimpleFsSDeviceException e = header->LatestStatus;

   switch (e)
   {
      case SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY:

         if(ProcessUnhandledThrowMustBeCalled)
            TEST_PASS_MESSAGE("Test pass, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY));
         else
            TEST_FAIL_MESSAGE("Test fail, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY));
         break;

      case SIMPLE_FS_SDEVICE_EXCEPTION_BAD_AREA_OVERFLOW:

         if(ProcessUnhandledThrowMustBeCalled)
            TEST_PASS_MESSAGE("Test pass, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_EXCEPTION_BAD_AREA_OVERFLOW));
         else
            TEST_FAIL_MESSAGE("Test fail, trown " ENUM_TO_STRING(SIMPLE_FS_SDEVICE_EXCEPTION_BAD_AREA_OVERFLOW));
         break;
   }
}

void SDeviceProcessLogStatus(const void *_handle)
{
   SDeviceHandleHeader *header = (SDeviceHandleHeader *)(_handle);
   SimpleFsSDeviceStatus status = header->LatestStatus;

   switch (status)
   {
      case SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED:

         TEST_MESSAGE(ENUM_TO_STRING(SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED));
         break;

      case SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED:
         TEST_MESSAGE(ENUM_TO_STRING(SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED));
         break;

      default:
         break;
   }
}

void* SDeviceMalloc(size_t size)
{
   void *memory = malloc(size);

   if (memory == NULL)
      SDeviceThrow(CoreGlobalSDeviceHandle, CORE_GLOBAL_SDEVICE_EXCEPTION_OUT_OF_MEMORY);

   return memory;
}

void SDeviceFree(void *_handle)
{
   free(_handle);
}
