#pragma once

#include "../selection_filter_interface.h"

SELECTION_FILTER_CONTEXT_FORWARD_DECLARATION(ValidServiceOfType);
SELECTION_FILTER_PARAMETERS_FORWARD_DECLARATION(ValidServiceOfType);

struct SELECTION_FILTER_CONTEXT(ValidServiceOfType)
{

};

struct SELECTION_FILTER_PARAMETERS(ValidServiceOfType)
{
   BlockType Type;
};

const extern SelectionFilterInterface SELECTION_FILTER_INTERFACE(ValidServiceOfType);
