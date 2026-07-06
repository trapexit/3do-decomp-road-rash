#include "item.h"
/* Reconstructed from the original function at 0x00035F00. */



#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"



MemoryHeapDescriptor *
find_memory_bank_containing_address(uint32 address)
{
  MemoryHeapDescriptor *heap;
  uint32 start;
  uint32 end;

  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  if(heap == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  do
    {
      start = (uint32)heap->base_address;
      if(address >= start)
        {
          end = start + heap->size;
          if(end >= address)
            {
              return heap;
            }
        }
      heap = heap->next;
    } while(heap != 0);

  return 0;
}
