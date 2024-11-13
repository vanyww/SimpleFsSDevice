#pragma once

#include "../Src/IO/Primitives/Block/Base/DataTypes/Enumerations/sector_state.h"
#include "../Src/IO/Primitives/Block/Base/DataTypes/block.h"

#include <stdbool.h>

Block CreateHeaderBlock(SectorState state, bool isMemoryErasingToZero);
