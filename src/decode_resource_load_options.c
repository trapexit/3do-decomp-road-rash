/* Reconstructed from the 0x00039D50 resource option decoder. */

#include "resource_manager_runtime.h"

void
decode_resource_load_options(uint32  options,
                             uint32 *out_memory_type,
                             uint32 *out_handle_options)
{
  uint32 memory_type;
  uint32 memory_selection;
  uint32 handle_options;

  memory_type = MEMORY_REQUEST_ANY;
  memory_selection = options & RESOURCE_LOAD_MEMORY_MASK;
  if(memory_selection != 0 &&
     memory_selection != RESOURCE_LOAD_MEMORY_MASK)
    {
      if((options & RESOURCE_LOAD_MEMORY_VRAM) == 0)
        {
          memory_type = MEMORY_REQUEST_DRAM;
        }
      else
        {
          memory_type = MEMORY_REQUEST_VRAM;
        }
    }

  handle_options = MEM_POINTER_OPTIONS_DEFAULT;
  if((options & RESOURCE_LOAD_OPTION_ALLOW_FAILURE) != 0)
    {
      handle_options = MEM_POINTER_OPTION_ALLOW_FAILURE;
    }
  if((options & RESOURCE_LOAD_OPTION_COMPACT_FIRST) != 0)
    {
      handle_options |= MEM_POINTER_OPTION_COMPACT_FIRST;
    }
  if((options & RESOURCE_LOAD_OPTION_COMPACT_HEAP_FIRST) != 0)
    {
      handle_options |= MEM_ALLOCATION_OPTION_PURGE_FIRST;
    }
  if((options & RESOURCE_LOAD_OPTION_KEEP_BUSY) != 0)
    {
      handle_options |= MEM_HANDLE_OPTION_PURGEABLE;
    }

  *out_handle_options = handle_options;
  *out_memory_type = memory_type;
}
