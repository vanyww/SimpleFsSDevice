#include "test_device.h"
#include "Mock/Functions/mock_functions.h"

#include <memory.h>

static const __SDEVICE_CONSTANT_DATA(FlashFileSystem) ConstantData =
{
   .TryEraseSector = TryEraseMemorySector,
   .TryRead = TryReadFromMemory,
   .TryWrite = TryWriteToMemory,
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
};

void CreateFlashFileSystemSDevice(__SDEVICE_HANDLE(FlashFileSystem) *handle, bool clearMemory)
{
   handle->Constant = &ConstantData;

   if(clearMemory)
      memset(MockMemorySectors, 0xFF, sizeof(MockMemorySectors));

   __SDEVICE_INITIALIZE_HANDLE(FlashFileSystem)(handle);
}
