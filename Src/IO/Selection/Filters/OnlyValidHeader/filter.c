#include "filter.h"
#include "../../../Primitives/Block/header_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(OnlyValidHeader);

static SELECTION_FILTER_FUNCTION_DECLARATION(OnlyValidHeader, handle, _parameters, _context, block)
{
   HeaderBlock blockAsHeader = block.AsHeader;
   if(HasHeaderValidSectorState(blockAsHeader) && HasHeaderCompatibleVersion(blockAsHeader))
      return (FilteringResult){ -1, true };

   SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED);

   return (FilteringResult){ -1, false };
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(OnlyValidHeader) =
{
   .FilterFunction      = SELECTION_FILTER_FUNCTION(OnlyValidHeader),
   .ContextInitFunction = NULL
};
