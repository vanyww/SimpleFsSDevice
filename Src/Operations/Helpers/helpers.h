#pragma once

#include "../Base/block.h"

void SimpleFsSDeviceSetSectorHeaderState(__SDEVICE_HANDLE(SimpleFs) *, Iterator *, SectorHeaderState);
bool SimpleFsSDeviceTryGetSectorHeaderState(__SDEVICE_HANDLE(SimpleFs) *, Iterator *, SectorHeaderState *);
void SimpleFsSDeviceFormatSectorToState(__SDEVICE_HANDLE(SimpleFs) *, Iterator *, SectorHeaderState);
bool SimpleFsSDeviceTryMoveFileDataToCache(__SDEVICE_HANDLE(SimpleFs) *, SimpleFsSDeviceAddress);
void SimpleFsSDeviceTransferSectors(__SDEVICE_HANDLE(SimpleFs) *);
void SimpleFsSDeviceClearMemoryState(__SDEVICE_HANDLE(SimpleFs) *);
