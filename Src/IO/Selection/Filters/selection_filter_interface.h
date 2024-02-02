#pragma once

#include "../../../private.h"
#include "../../Primitives/Block/Base/DataTypes/block.h"

typedef struct
{
   intptr_t NextBlockOffset;
   bool     IsBlockSelected;
} FilteringResult;

#define SELECTION_FILTER_FUNCTION_RETURN_VALUE FilteringResult
#define SELECTION_FILTER_FUNCTION_ARGUMENTS(handle_name, parameters_name, context_name, block_name) (                  \
   ThisHandle *handle_name,                                                                                            \
   const void *parameters_name,                                                                                        \
   void *context_name,                                                                                                 \
   Block block_name)
#define SELECTION_FILTER_FUNCTION_POINTER(pointer_name)                                                                \
   SELECTION_FILTER_FUNCTION_RETURN_VALUE                                                                              \
   (* pointer_name)                                                                                                    \
   SELECTION_FILTER_FUNCTION_ARGUMENTS(,,,)
#define SELECTION_FILTER_FUNCTION(filter_name)                                                                         \
   _SimpleFsSDeviceInternal##filter_name##SelectionFilterFunction
#define SELECTION_FILTER_FUNCTION_DECLARATION(filter_name, handle_name, parameters_name, context_name, block_name)     \
   SELECTION_FILTER_FUNCTION_RETURN_VALUE                                                                              \
   SELECTION_FILTER_FUNCTION(filter_name)                                                                              \
   SELECTION_FILTER_FUNCTION_ARGUMENTS(handle_name, parameters_name, context_name, block_name)

#define SELECTION_FILTER_CONTEXT_INIT_FUNCTION_RETURN_VALUE void
#define SELECTION_FILTER_CONTEXT_INIT_FUNCTION_ARGUMENTS(handle_name, parameters_name, context_name) (                 \
   ThisHandle *handle_name,                                                                                            \
   const void *parameters_name,                                                                                        \
   void *context_name)
#define SELECTION_FILTER_CONTEXT_INIT_FUNCTION_POINTER(pointer_name)                                                   \
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION_RETURN_VALUE                                                                 \
   (* pointer_name)                                                                                                    \
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION_ARGUMENTS(,,)
#define SELECTION_FILTER_CONTEXT_INIT_FUNCTION(filter_name)                                                            \
   _SimpleFsSDeviceInternal##filter_name##SelectionFilterContextInitFunction
#define SELECTION_FILTER_CONTEXT_INIT_FUNCTION_DECLARATION(filter_name, handle_name, parameters_name, context_name)    \
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION_RETURN_VALUE                                                                 \
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION(filter_name)                                                                 \
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION_ARGUMENTS(handle_name, parameters_name, context_name)

typedef struct
{
   SELECTION_FILTER_FUNCTION_POINTER(FilterFunction);
   SELECTION_FILTER_CONTEXT_INIT_FUNCTION_POINTER(ContextInitFunction);
} SelectionFilterInterface;

#define SELECTION_FILTER_INTERFACE(filter_name)                                                                        \
   _SimpleFsSDeviceInternal##filter_name##SelectionFilterInterface

#define SELECTION_FILTER_CONTEXT(filter_name)                                                                          \
   _SimpleFsSDeviceInternal##filter_name##SelectionFilterContext
#define SELECTION_FILTER_CONTEXT_FORWARD_DECLARATION(filter_name)                                                      \
   typedef struct SELECTION_FILTER_CONTEXT(filter_name) SELECTION_FILTER_CONTEXT(filter_name)

#define SELECTION_FILTER_PARAMETERS(filter_name)                                                                       \
   _SimpleFsSDeviceInternal##filter_name##SelectionFilterParameters
#define SELECTION_FILTER_PARAMETERS_FORWARD_DECLARATION(filter_name)                                                   \
   typedef struct SELECTION_FILTER_PARAMETERS(filter_name) SELECTION_FILTER_PARAMETERS(filter_name)

#define SELECTION_FILTER_INTERNAL_ALIASES_DECLARATION(filter_name)                                                     \
   typedef SELECTION_FILTER_CONTEXT(filter_name) ThisSelectorFilterContext;                                            \
   typedef SELECTION_FILTER_PARAMETERS(filter_name) ThisSelectorFilterParameters;

typedef struct
{
   const SelectionFilterInterface *Interface;
   const void                     *Parameters;
   void                           *Context;
} SelectionFilter;

#define COMPOSE_SELECTION_FILTER(filter_name, ...) (SelectionFilter)                                                   \
{                                                                                                                      \
   .Interface  = &SELECTION_FILTER_INTERFACE(filter_name),                                                             \
   .Parameters = &(const SELECTION_FILTER_PARAMETERS(filter_name)){ __VA_ARGS__ },                                     \
   .Context    = &(SELECTION_FILTER_CONTEXT(filter_name)){ }                                                           \
}
