#include "filter.h"
#include "../../../Primitives/Block/Base/common_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(ExceptEmpty);

static SELECTION_FILTER_FUNCTION_DECLARATION(ExceptEmpty, handle, _parameters, _context, block)
{
   SDeviceDebugAssert(handle != NULL);

   return (FilteringResult){ -1, !IsBlockEmpty(handle, block) };
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(ExceptEmpty) =
{
   .FilterFunction = SELECTION_FILTER_FUNCTION(ExceptEmpty),
   .ContextInitFunction = NULL
};
