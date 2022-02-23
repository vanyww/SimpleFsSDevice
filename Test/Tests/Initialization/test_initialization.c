#include "test_initialization.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"
#include "../../Device/Mock/RuntimeError/mock_handle_runtime_error.h"

bool TestHandleInitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) handle = CreateFlashFileSystemSDevice(true);

   if(handle.IsInitialized != true)
      return false;

   if(WasAssertFailed() == true)
      return false;

   if(WasRuntimeErrorRaised() == true)
      return false;

   return true;
}
