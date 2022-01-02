#pragma once

#include "Base/block.h"

#define __RETURN_ERROR_IF_ANY(expression) (                                                                            \
{                                                                                                                      \
   FlashFileSystemState _state = expression;                                                                           \
   if(_state != FLASH_FILE_SYSTEM_STATE_OK)                                                                            \
      return _state;                                                                                                   \
})

typedef struct
{
   BlockHeaderState HeaderState;
   bool HasValidHeaderState;
   bool IsEmpty;
} SectorInitialState;

FlashFileSystemState SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                          FlashFileSystemIterator *,
                                          BlockHeaderState);

FlashFileSystemState GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                          FlashFileSystemIterator *,
                                          BlockHeaderState *);

FlashFileSystemState FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                         FlashFileSystemIterator *,
                                         BlockHeaderState);

FlashFileSystemState GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           FlashFileSystemIterator *,
                                           SectorInitialState *);

FlashFileSystemState MoveVariableDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress);
FlashFileSystemState TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *);
