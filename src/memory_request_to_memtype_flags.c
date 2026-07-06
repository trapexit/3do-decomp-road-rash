#include "memory_manager_runtime.h"

uint32
memory_request_to_memtype_flags(uint32 memory_request)
{
  if(memory_request == MEMORY_REQUEST_DRAM)
    {
      return MEMTYPE_DRAM;
    }
  if(memory_request == MEMORY_REQUEST_VRAM)
    {
      return MEMTYPE_VRAM;
    }
  if(memory_request == MEMORY_REQUEST_ANY)
    {
      return MEMTYPE_ANY;
    }
  return memory_request;
}
