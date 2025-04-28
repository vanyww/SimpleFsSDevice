#pragma once

#include "SDeviceCore/config.h"

#if !defined(SIMPLE_FS_SDEVICE_USE_EXTERNAL_CONFIG) || defined(DOXYGEN)
   #define SIMPLE_FS_SDEVICE_USE_EXTERNAL_CONFIG false
#endif

#if SIMPLE_FS_SDEVICE_USE_EXTERNAL_CONFIG
   #include "simple_fs_sdevice_config.h"
#endif

#if !defined(SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC) || defined(DOXYGEN)
   #define SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC false
#endif
