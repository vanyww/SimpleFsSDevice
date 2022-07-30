#pragma once

#include "../private.h"

void SimpleFsSDeviceWriteFile(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress, const void *, size_t, bool);
void SimpleFsSDeviceProcessInitialState(__SDEVICE_HANDLE(SimpleFs) *);
