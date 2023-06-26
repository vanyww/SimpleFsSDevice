#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "../Inc/SimpleFsSDevice/public.h"


void SetProcessAssertFailMustBeCalled(bool value);
void SetProcessUnhandledThrowMustBeCalled(bool value);
void SetAssertFailhandle(SDEVICE_HANDLE(SimpleFs) *handle);

void SDeviceProcessAssertFail(char *file, int line);
void SDeviceProcessUnhandledThrow(const void *_handle);
void SDeviceProcessLogStatus(const void *_handle);
void* SDeviceMalloc(size_t size);
void SDeviceFree(void *_handle);
