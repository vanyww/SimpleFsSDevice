#pragma once

#include "../../Inc/FlashFileSystemSDevice/core.h"
#include "Primitives/sector.h"

#define __RETURN_ERROR_IF_ANY(expression)                                                                              \
({                                                                                                                     \
   FlashFileSystemSDeviceState state = expression;                                                                     \
   if(state != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)                                                                     \
      return state;                                                                                                    \
})

typedef struct
{
   intptr_t FirstEmptyBlockAddress;
   BlockHeaderState HeaderState;
   bool HasValidState;
   bool IsEmpty;
} SectorInitialState;

FlashFileSystemSDeviceState ReadBlockToBuffer(__SDEVICE_HANDLE(FlashFileSystem) *, intptr_t);

FlashFileSystemSDeviceState WriteBlockFromBuffer(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                 FlashFileSystemSDeviceSectorDynamicData *);

FlashFileSystemSDeviceState GetSectorState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           const FlashFileSystemSDeviceSector *,
                                           SectorInitialState *);

FlashFileSystemSDeviceState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                FlashFileSystemSDeviceSectorDynamicData *,
                                                BlockHeaderState);

FlashFileSystemSDeviceState SetSectorState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           FlashFileSystemSDeviceSectorDynamicData *,
                                           BlockHeaderState);

FlashFileSystemSDeviceState CachePreambleBlockDataWithAddress(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                              FlashFileSystemSDeviceAddress);

FlashFileSystemSDeviceState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *);
