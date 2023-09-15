#pragma once

#include "Base/service_block.h"

static inline HeaderBlock BuildHeaderBlock(ThisHandle *handle, SectorState sectorState)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(IS_VALID_SECTOR_STATE(sectorState));

   HeaderBlock block =
   {
      .Type = BLOCK_TYPE_HEADER,
      .SectorState = sectorState,
      .FsVersion = SDEVICE_IDENTITY_BLOCK(SimpleFs).Version,
      .Padding = GetEmptyUInt8MemoryValue(handle)
   };
   block.BlockCrc = ComputeServiceBlockCrc(handle, block);

   return block;
}

static inline bool IsHeaderBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_HEADER);
}

static inline bool HasHeaderCompatibleVersion(HeaderBlock block)
{
   SDeviceDebugAssert(IsHeaderBlock(block));

   return block.FsVersion.Major == SIMPLE_FS_SDEVICE_VERSION_MAJOR;
}

static inline bool HasHeaderValidSectorState(HeaderBlock block)
{
   SDeviceDebugAssert(IsHeaderBlock(block));

   return IS_VALID_SECTOR_STATE(block.SectorState);
}
