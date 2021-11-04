#include "mock_handle_runtime_error.h"

#include <stdint.h>

static bool WasRuntimeErrorRaisedFlag = false;

bool WasRuntimeErrorRaised(void)
{
   bool state = WasRuntimeErrorRaisedFlag;
   WasRuntimeErrorRaisedFlag = false;
   return state;
}

void SDeviceProcessRuntimeError(void *, int32_t)
{
   WasRuntimeErrorRaisedFlag = true;
}
