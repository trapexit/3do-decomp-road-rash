#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void
append_memory_region(MemoryRegionHeader *region,
                     void               *base_address,
                     uint32              size)
{
  MemoryRegionHeader *tail;

  tail = (MemoryRegionHeader *)memory_allocator_globals.block_list_tail;
  if(tail == 0)
    {
      memory_allocator_globals.block_list_tail = region;
    }
  else
    {
      while(tail->next != 0)
        {
          tail = tail->next;
        }
      tail->next = region;
    }

  region->base_address = base_address;
  region->size = size;
  region->next = 0;
}
