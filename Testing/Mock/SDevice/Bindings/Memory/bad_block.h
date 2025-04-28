#pragma once

#include "stddef.h"

typedef struct
{
   size_t *PrimarySectorBadBlocksIdxs;
   size_t *AuxiliarySectorBadBlocksIdxs;
   size_t  PrimarySectorBadBlocksIdxsCount;
   size_t  AuxiliarySectorBadBlocksIdxsCount;
} MockBadBlocks;

void SetMockBadBlocks(MockBadBlocks blocks);
void GetMockBadBlocks(MockBadBlocks *blocks);
