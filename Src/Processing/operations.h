#pragma once

#include "../../Inc/FlashFileSystemSDevice/core.h"
#include "Base/block.h"

#define __RETURN_ERROR_IF_ANY(expression) (                                                                            \
{                                                                                                                      \
   FlashFileSystemSDeviceState state = expression;                                                                     \
   if(state != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)                                                                     \
      return state;                                                                                                    \
})

typedef struct
{
   BlockHeaderState HeaderState;
   bool HasValidHeaderState;
   bool IsEmpty;
} SectorInitialState;

FlashFileSystemSDeviceState SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                 FlashFileSystemSDeviceIterator *,
                                                 BlockHeaderState);

FlashFileSystemSDeviceState GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                 FlashFileSystemSDeviceIterator *,
                                                 BlockHeaderState *);

FlashFileSystemSDeviceState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                FlashFileSystemSDeviceIterator *,
                                                BlockHeaderState);

FlashFileSystemSDeviceState GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                  FlashFileSystemSDeviceIterator *,
                                                  SectorInitialState *);

FlashFileSystemSDeviceState MoveVariableDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                    FlashFileSystemSDeviceAddress);

FlashFileSystemSDeviceState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *);
