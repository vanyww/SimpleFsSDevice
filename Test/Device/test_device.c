#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

__SDEVICE_HANDLE(FlashFileSystem) CreateFlashFileSystemSDevice(bool clearMemory)
{
   __SDEVICE_HANDLE(FlashFileSystem) handle =
   {
      .Init = (__SDEVICE_INIT_DATA(FlashFileSystem))
      {
         .TryEraseSector = TryEraseMemorySector,
         .TryReadBlock = TryReadFromMemory,
         .TryWriteBlock = TryWriteToMemory,
         .MaxUsedAddress = __MAX_USED_ADDRESS,
         .Sectors =
         {
            {
               .StartAddress = (intptr_t)&MockMemorySectors[0],
               .Size = __MOCK_MEMORY_SECTOR_SIZE
            },
            {
               .StartAddress = (intptr_t)&MockMemorySectors[1],
               .Size = __MOCK_MEMORY_SECTOR_SIZE
            }
         }
      }
   };

   if(clearMemory)
      memset(MockMemorySectors, 0xFF, sizeof(MockMemorySectors));

   __SDEVICE_INITIALIZE_HANDLE(FlashFileSystem)(&handle);

   return handle;
}
