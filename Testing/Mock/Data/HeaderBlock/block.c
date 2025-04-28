#include "block.h"

#include "../../../../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"

Block MockCreateHeaderBlock(SectorState state)
{
   HeaderBlock block =
   {
      .Type        = BLOCK_TYPE_HEADER,
      .SectorState = state,
      .Padding     = 0x00,
      .FsVersion   =
      {
         .Major = SIMPLE_FS_SDEVICE_VERSION_MAJOR,
         .Minor = SIMPLE_FS_SDEVICE_VERSION_MINOR,
         .Patch = SIMPLE_FS_SDEVICE_VERSION_PATCH
      }
   };

   uint8_t *blockdata = ((ServiceBlock)block).BlockData;
   size_t size = sizeof(((ServiceBlock)block).BlockData);
   block.BlockCrc = TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, blockdata, size);

   return (Block)block;
}
