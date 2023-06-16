#pragma once

#include <stdlib.h>
#include <stdbool.h>

extern bool ProcessAssertFailMustBeCalled;
extern bool ProcessLogStatusMustBeCalled;
extern bool ProcessUnhandledThrowMustBeCalled;

void SDeviceProcessAssertFail(char *file, int line);
void SDeviceProcessUnhandledThrow(const void *_handle);
void SDeviceProcessLogStatus(const void *_handle);
void* SDeviceMalloc(size_t size);
void SDeviceFree(void *_handle);
