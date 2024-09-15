#include "block.h"

#include "SimpleFsSDevice/public.h"
#include "../Src/IO/Primitives/Block/Base/Common/Crc/crc.h"

Block CreateHeaderBlock(SectorState state, bool isMemoryErasingToZero)
{
   HeaderBlock block =
   {
      .Type = BLOCK_TYPE_HEADER,
      .SectorState = state,
      .FsVersion = SDEVICE_IDENTITY_BLOCK(SimpleFs).Version,
      .Padding = isMemoryErasingToZero ? 0 : UINT8_MAX,
   };

   uint8_t *blockdata = ((ServiceBlock)block).BlockData;
   size_t size = sizeof(((ServiceBlock)block).BlockData);
   block.BlockCrc = TableCrc8SDeviceCompute(SimpleFsSDeviceInternalCrc8Handle, blockdata, size);

   return (Block)block;
}
