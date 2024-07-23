#include "crc.h"

#if !SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
SDEVICE_HANDLE(TableCrc8) *$SimpleFsSDeviceCrc8Handle = NULL;
SDEVICE_HANDLE(TableCrc16) *$SimpleFsSDeviceCrc16Handle = NULL;
#endif
