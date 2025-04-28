#pragma once

#include "SDeviceCore/core.h"

typedef void (* AssertFailHandler)(void);
typedef void (* PanicHandler)(const void *handle, SDevicePanic panic);

void SetAssertFailHandler(AssertFailHandler handler);
void SetPanicHandler(PanicHandler handler);
void ResetAssertFailHandler(void);
void ResetPanicHandler(void);
