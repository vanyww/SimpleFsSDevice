#pragma once

#include "Base/service_block.h"

static inline HeaderBlock BuildHeaderBlock(ThisHandle *handle, SectorState sectorState)
{
   HeaderBlock block =
   {
      .Type        = BLOCK_TYPE_HEADER,
      .SectorState = sectorState,
      .Padding     = GetEmptyUInt8MemoryValue(handle),
      .FsVersion   =
      {
         .Major = SIMPLE_FS_SDEVICE_VERSION_MAJOR,
         .Minor = SIMPLE_FS_SDEVICE_VERSION_MINOR,
         .Patch = SIMPLE_FS_SDEVICE_VERSION_PATCH
      }
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
   return block.FsVersion.Major == SIMPLE_FS_SDEVICE_VERSION_MAJOR;
}

static inline bool HasHeaderValidSectorState(HeaderBlock block)
{
   return IS_VALID_SECTOR_STATE(block.SectorState);
}
