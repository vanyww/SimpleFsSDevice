#include "bad_block.h"

MockBadBlocks ThisBadBlocks;

void SetMockBadBlocks(MockBadBlocks blocks)
{
   ThisBadBlocks = blocks;
}

void GetMockBadBlocks(MockBadBlocks *blocks)
{
   *blocks = ThisBadBlocks;
}
