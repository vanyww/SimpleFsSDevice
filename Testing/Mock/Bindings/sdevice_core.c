#include "sdevice_core.h"

#include "SimpleFsSDevice/public.h"

#include "SDeviceCore/common.h"
#include "CoreGlobalSDevice/public.h"

#include "unity_fixture.h"

#include <stdlib.h>
#include <stdio.h>

#define ASSERT_FAIL_FORMAT "Assert failed on file (%s) line (%d)"
#define PANIC_THROWN_FORMAT "Thrown panic "

#define ENUM_TO_STRING(enum) #enum

AssertFailHandler ThisAssertFailHandler;
PanicHandler ThisPanicHandler;

void SetAssertFailHandler(AssertFailHandler handler)
{
   ThisAssertFailHandler = handler;
}

void SetPanicHandler(PanicHandler handler)
{
   ThisPanicHandler = handler;
}

void ResetAssertFailHandler(void)
{
   ThisAssertFailHandler = NULL;
}

void ResetPanicHandler(void)
{
   ThisPanicHandler = NULL;
}

void * SDeviceAllocateMemory(size_t size)
{
   if(size <= 0)
      return NULL;

   void *memoryPointer = malloc(size);

   if(!memoryPointer)
      CoreGlobalSDeviceThrowPanic(CORE_GLOBAL_SDEVICE_PANIC_OUT_OF_MEMORY);

   return memoryPointer;
}

void SDeviceFreeMemory(void *pointer)
{
   free(pointer);
}

void SDeviceProcessAssertFail(char *file, int line)
{
   int messageLength = snprintf(NULL, 0, ASSERT_FAIL_FORMAT, file, line);
   char message[messageLength + 1];
   sprintf(message, ASSERT_FAIL_FORMAT, file, line);

   TEST_MESSAGE(message);

   if(ThisAssertFailHandler)
   {
      ThisAssertFailHandler();
   }
   else
   {
      TEST_FAIL_MESSAGE("Assert fail has not been processed");
   }
}

void SDeviceProcessPanic(const void *handle, SDevicePanic panic)
{
   switch (panic)
   {
      case SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY:
         TEST_MESSAGE(PANIC_THROWN_FORMAT ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY));
         break;

      case SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW:
         TEST_MESSAGE(PANIC_THROWN_FORMAT ENUM_TO_STRING(SIMPLE_FS_SDEVICE_PANIC_BAD_AREA_OVERFLOW));
         break;
   }

   if(ThisPanicHandler)
   {
      ThisPanicHandler(handle, panic);
   }
   else
   {
      TEST_FAIL_MESSAGE("Panic has not been processed");
   }
}
