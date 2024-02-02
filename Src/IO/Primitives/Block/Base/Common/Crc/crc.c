#include "crc.h"

#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
SDEVICE_HANDLE(TableCrc8) *SimpleFsSDeviceInternalCrc8Handle = NULL;
SDEVICE_HANDLE(TableCrc16) *SimpleFsSDeviceInternalCrc16Handle = NULL;
#endif
