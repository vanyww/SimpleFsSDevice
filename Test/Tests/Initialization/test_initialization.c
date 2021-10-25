#include "test_initialization.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"

bool TestHandleInitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) handle;
   CreateFlashFileSystemSDevice(&handle, true);

   if(handle.IsInitialized != true)
      return false;

   if(WasAssertFailed() == true)
      return false;

   return true;
}
