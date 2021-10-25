#include "mock_assert.h"

static bool WasAssertFailedFlag = false;

bool WasAssertFailed(void)
{
   bool state = WasAssertFailedFlag;
   WasAssertFailedFlag = false;
   return state;
}

void SDeviceAssertFailed(char *, int)
{
   WasAssertFailedFlag = true;
}
