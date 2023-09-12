#include "CoreGlobalSDevice/public.h"

#include <stdlib.h>

void* SDeviceMalloc(size_t size)
{
   void *memory = malloc(size);

   if (memory == NULL)
      CoreGlobalSDeviceThrowPanic(CORE_GLOBAL_SDEVICE_PANIC_OUT_OF_MEMORY);

   return memory;
}

void SDeviceFree(void *_handle)
{
   free(_handle);
}
