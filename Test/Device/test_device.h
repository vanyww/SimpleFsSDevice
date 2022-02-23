#pragma once

#include "FlashFileSystemSDevice/core.h"

#define __MAX_USED_ADDRESS 10

__SDEVICE_HANDLE(FlashFileSystem) CreateFlashFileSystemSDevice(bool);
