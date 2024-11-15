#include "filter.h"
#include "../../../Primitives/Block/bad_area_tag_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(ExceptBadAreas);

static SELECTION_FILTER_FUNCTION_DECLARATION(ExceptBadAreas, handle, parameters, context, block)
{
   ThisSelectorFilterContext *_context = (ThisSelectorFilterContext *)context;

   if(HasBlockValidType(block) && IsBadAreaTagBlock(block))
   {
      ServiceBlock blockAsService = block.AsService;

      if(HasServiceBlockValidCrc(handle, block.AsService))
      {
         BadAreaTagBlock blockAsBadAreaTag = blockAsService.AsBadAreaTag;
         return (FilteringResult){ -(blockAsBadAreaTag.BadAreaLength + 1), false };
      }

      SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED);

      _context->IsBadBlockSkipOngoing = true;

      return (FilteringResult){ -1, false };
   }

   if(_context->IsBadBlockSkipOngoing)
   {
      if(!IsServiceBlock(block))
         return (FilteringResult){ -1, false };

      _context->IsBadBlockSkipOngoing = false;
   }

   return (FilteringResult){ -1, true };
}

static SELECTION_FILTER_CONTEXT_INIT_FUNCTION_DECLARATION(ExceptBadAreas, handle, parameters, context)
{
   ThisSelectorFilterContext *_context = (ThisSelectorFilterContext *)context;

   _context->IsBadBlockSkipOngoing = false;
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(ExceptBadAreas) =
{
   .FilterFunction      = SELECTION_FILTER_FUNCTION(ExceptBadAreas),
   .ContextInitFunction = SELECTION_FILTER_CONTEXT_INIT_FUNCTION(ExceptBadAreas)
};
