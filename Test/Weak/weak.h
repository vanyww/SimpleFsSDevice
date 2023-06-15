#pragma once

#include <stdlib.h>


void SDeviceProcessAssertFail(char *file, int line);
void SDeviceProcessUnhandledThrow(const void *_handle);
void SDeviceProcessLogStatus(const void *_handle);
void* SDeviceMalloc(size_t size);
void SDeviceFree(void *_handle);
