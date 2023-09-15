#pragma once

#include "../Inc/SimpleFsSDevice/public.h"

#include <stdbool.h>

void AssertionMustBeFail(bool value);
void PanicMustBeThrown(bool value);
void SetAssertFailHandle(SDEVICE_HANDLE(SimpleFs) *handle);
void ShowLog(bool value);
