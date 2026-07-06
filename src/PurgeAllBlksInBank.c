/* PurgeAllBlksInBank - original function at 0x000355B4. */



#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void
PurgeAllBlksInBank(MemoryHeapDescriptor *heap)
{
  MemoryHandle *handle;
  MemoryHandle *previous;
  int purged_count;

  purged_count = 0;
  previous = 0;
  handle = (MemoryHandle *)memory_allocator_globals.block_list_head;
  while(handle != 0)
    {
      handle = PurgeOneHandle(
        heap->memory_type, &heap, handle, &previous,
        &purged_count, MEM_POINTER_OPTIONS_DEFAULT);
    }
}
