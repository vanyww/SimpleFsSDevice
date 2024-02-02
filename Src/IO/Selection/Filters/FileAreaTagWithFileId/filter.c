#include "filter.h"
#include "../../../Primitives/Block/File/file_area_tag_block.h"

SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(FileAreaTagWithFileId);

static SELECTION_FILTER_FUNCTION_DECLARATION(FileAreaTagWithFileId, handle, _parameters, _context, block)
{
   const ThisSelectorFilterParameters *parameters = (ThisSelectorFilterParameters *)_parameters;

   FileAreaTagBlock blockAsFileAreaTag = block.AsFileAreaTag;
   return (FilteringResult){ -1, blockAsFileAreaTag.FileIdx == parameters->FileIdx };
}

const SelectionFilterInterface SELECTION_FILTER_INTERFACE(FileAreaTagWithFileId) =
{
   .FilterFunction      = SELECTION_FILTER_FUNCTION(FileAreaTagWithFileId),
   .ContextInitFunction = NULL
};
