#include "filter.h"
#include "../../../Primitives/Block/Base/area_tag_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(ValidServiceOfType);

static SELECTION_FILTER_FUNCTION_DECLARATION(ValidServiceOfType, handle, _parameters, _context, block)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(_parameters != NULL);

   const ThisSelectorFilterParameters *parameters = (ThisSelectorFilterParameters *)_parameters;

   SDeviceDebugAssert(IS_VALID_SERVICE_BLOCK_TYPE(parameters->Type));

   if(HasBlockValidType(block))
   {
      if(!IsServiceBlock(block))
         return (FilteringResult){ -1, false };

      ServiceBlock blockAsService = block.AsService;
      if(HasServiceBlockValidCrc(handle, blockAsService))
      {
         intptr_t nextOffset = IsAreaTagBlock(block) ? -block.AsAreaTag.AreaLength : -1;
         return (FilteringResult){ nextOffset, IsBlockOfType(block, parameters->Type) };
      }
   }

   SDeviceLogStatus(handle, SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED);
   return (FilteringResult){ -1, false };
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(ValidServiceOfType) =
{
   .FilterFunction = SELECTION_FILTER_FUNCTION(ValidServiceOfType),
   .ContextInitFunction = NULL
};