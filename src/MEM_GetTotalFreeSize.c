#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"



int
MEM_GetTotalFreeSize(uint32 memory_request)
{
  uint32 mask;
  int total;
  MemoryHeapDescriptor *heap;

  total = 0;
  mask = memory_request_to_memtype_flags(memory_request);
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;

  if(heap == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      return MEM_ERROR_NO_MEMORY_BANK;
    }

  while(heap != 0)
    {
      if(mask == 0 || (heap->memory_type & mask) != 0)
        {
          total += (int)heap->free_bytes;
        }
      heap = heap->next;
    }

  return total;
}
