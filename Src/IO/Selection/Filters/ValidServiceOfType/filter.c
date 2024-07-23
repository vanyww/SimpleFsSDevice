#include "filter.h"
#include "../../../Primitives/Block/Base/area_tag_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(ValidServiceOfType);

static SELECTION_FILTER_FUNCTION_DECLARATION(ValidServiceOfType, handle, parameters, context, block)
{
   const ThisSelectorFilterParameters *_parameters = (ThisSelectorFilterParameters *)parameters;

   if(HasBlockValidType(block))
   {
      if(!IsServiceBlock(block))
         return (FilteringResult){ -1, false };

      ServiceBlock blockAsService = block.AsService;

      if(HasServiceBlockValidCrc(handle, blockAsService))
      {
         intptr_t nextOffset = (IsAreaTagBlock(block)) ? -(block.AsAreaTag.AreaLength + 1) : -1;
         return (FilteringResult){ nextOffset, IsBlockOfType(block, _parameters->Type) };
      }
   }

   SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED);
   return (FilteringResult){ -1, false };
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(ValidServiceOfType) =
{
   .FilterFunction      = SELECTION_FILTER_FUNCTION(ValidServiceOfType),
   .ContextInitFunction = NULL
};
