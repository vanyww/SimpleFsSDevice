#include "CoreGlobalSDevice/public.h"

#include <stdlib.h>

void* SDeviceAllocateMemory(size_t size)
{
   void *memory = malloc(size);

   if (memory == NULL)
      CoreGlobalSDeviceThrowPanic(CORE_GLOBAL_SDEVICE_PANIC_OUT_OF_MEMORY);

   return memory;
}

void SDeviceFreeMemory(void *_handle)
{
   free(_handle);
}
