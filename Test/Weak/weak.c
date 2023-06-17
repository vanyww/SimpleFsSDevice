#include "weak.h"
#include "SDeviceCore/heap.h"
#include "SDeviceCore/errors.h"
#include "SDeviceCore/global.h"

#include "unity.h"
#include <stdio.h>

bool ProcessAssertFailMustBeCalled;
bool ProcessLogStatusMustBeCalled;
bool ProcessUnhandledThrowMustBeCalled;
SDEVICE_HANDLE(SimpleFs) *AssertFailhandle;


void SDeviceProcessAssertFail(char *file, int line)
{

   if(ProcessAssertFailMustBeCalled)
   {
      if(AssertFailhandle!= NULL)
            SDeviceFree(AssertFailhandle);

      char str[200];
      sprintf(str, "Test pass, assert called on file (%s) line (%d)", file, line);
      TEST_PASS_MESSAGE(str);
   }
   else
   {
      if(AssertFailhandle!= NULL)
            SDeviceFree(AssertFailhandle);

      char str[200];
      sprintf(str, "Test fail, assert called on file (%s) line (%d)", file,  line);
      TEST_FAIL_MESSAGE(str);
   }
}

void SDeviceProcessUnhandledThrow(const void *_handle) {}

void SDeviceProcessLogStatus(const void *_handle) {}

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
