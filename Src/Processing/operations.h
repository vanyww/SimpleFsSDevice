#pragma once

#include "Base/block.h"

#define __RETURN_ERROR_IF_ANY(expression) (                                                                            \
{                                                                                                                      \
   FlashFileSystemStatus _status = expression;                                                                         \
   if(_status != FLASH_FILE_SYSTEM_STATUS_OK)                                                                          \
      return _status;                                                                                                  \
})

typedef struct
{
   BlockHeaderState HeaderState;
   bool HasValidHeaderState;
   bool IsEmpty;
} SectorInitialState;

FlashFileSystemStatus SetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                          FlashFileSystemIterator *,
                                          BlockHeaderState);

FlashFileSystemStatus GetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                          FlashFileSystemIterator *,
                                          BlockHeaderState *);

FlashFileSystemStatus FormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                         FlashFileSystemIterator *,
                                         BlockHeaderState);

FlashFileSystemStatus GetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                           FlashFileSystemIterator *,
                                           SectorInitialState *);

FlashFileSystemStatus MoveVariableDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress);
FlashFileSystemStatus TransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *);
