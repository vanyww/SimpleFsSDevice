#pragma once

#include "../selection_filter_interface.h"

SELECTION_FILTER_CONTEXT_FORWARD_DECLARATION(ExceptBadAreas);
SELECTION_FILTER_PARAMETERS_FORWARD_DECLARATION(ExceptBadAreas);

struct SELECTION_FILTER_CONTEXT(ExceptBadAreas)
{
   bool IsBadBlockSkipOngoing;
};

struct SELECTION_FILTER_PARAMETERS(ExceptBadAreas)
{

};

const extern SelectionFilterInterface SELECTION_FILTER_INTERFACE(ExceptBadAreas);
