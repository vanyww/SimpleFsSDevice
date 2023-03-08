#pragma once

#include "service_block.h"

static inline bool IsAreaTagBlock(Block block)
{
   return IsBlockOfType(block, BLOCK_TYPE_ANY_AREA_TAG);
}
