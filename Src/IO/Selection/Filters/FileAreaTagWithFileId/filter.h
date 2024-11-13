#pragma once

#include "../selection_filter_interface.h"

SELECTION_FILTER_CONTEXT_FORWARD_DECLARATION(FileAreaTagWithFileId);
SELECTION_FILTER_PARAMETERS_FORWARD_DECLARATION(FileAreaTagWithFileId);

struct SELECTION_FILTER_CONTEXT(FileAreaTagWithFileId)
{

};

struct SELECTION_FILTER_PARAMETERS(FileAreaTagWithFileId)
{
   uint16_t FileIdx;
};

const extern SelectionFilterInterface SELECTION_FILTER_INTERFACE(FileAreaTagWithFileId);
