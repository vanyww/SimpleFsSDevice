#pragma once

#include "../Inc/SimpleFsSDevice/public.h"

#include <stdbool.h>

void SetProcessAssertFailMustBeCalled(bool value);
void SetProcessUnhandledThrowMustBeCalled(bool value);
void SetAssertFailHandle(SDEVICE_HANDLE(SimpleFs) *handle);
void ShowLog(bool value);

