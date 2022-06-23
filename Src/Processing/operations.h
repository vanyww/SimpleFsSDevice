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

FlashFileSystemStatus FlashFileSystemSetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                          FlashFileSystemIterator *,
                                                          BlockHeaderState);
FlashFileSystemStatus FlashFileSystemGetSectorHeaderState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                          FlashFileSystemIterator *,
                                                          BlockHeaderState *);
FlashFileSystemStatus FlashFileSystemFormatSectorToState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                         FlashFileSystemIterator *,
                                                         BlockHeaderState);
FlashFileSystemStatus FlashFileSystemGetSectorInitialState(__SDEVICE_HANDLE(FlashFileSystem) *,
                                                           FlashFileSystemIterator *,
                                                           SectorInitialState *);
FlashFileSystemStatus FlashFileSystemMoveFileDataToCache(__SDEVICE_HANDLE(FlashFileSystem) *, FlashFileSystemAddress);
FlashFileSystemStatus FlashFileSystemTransferSectors(__SDEVICE_HANDLE(FlashFileSystem) *);
void FlashFileSystemInvalidateFileDataCache(__SDEVICE_HANDLE(FlashFileSystem) *);
