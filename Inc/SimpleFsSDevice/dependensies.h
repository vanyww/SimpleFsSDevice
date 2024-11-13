#pragma once

#include "SDeviceCore/core.h"

#if (SDEVICE_CORE_VERSION_MAJOR != 9) || (SDEVICE_CORE_VERSION_MINOR < 0)
   #error SDevice core version is incorrect.
#endif

#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   #include "TableCrcSDevice/Crc16/public.h"
   #include "TableCrcSDevice/Crc8/public.h"

   #if (TABLE_CRC_SDEVICE_VERSION_MAJOR != 4) || (TABLE_CRC_SDEVICE_VERSION_MINOR < 0)
      #error Table CRC SDevice version is incorrect.
   #endif
#endif
