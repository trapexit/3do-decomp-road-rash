#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"




int
MEM_CompactHeap(uint32 options)
{
  MemoryHeapDescriptor *heap;
  uint32 target_options;
  uint32 target_vram;
  uint32 target_dram;
  uint32 purge_handles;
  uint32 memory_type;

  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;

  LockItem(memory_allocator_globals.lock_item, 1);

  target_options = options & MEM_COMPACT_TARGET_MASK;
  target_vram = options & MEM_COMPACT_TARGET_VRAM;
  target_dram = options & MEM_COMPACT_TARGET_DRAM;
  purge_handles = options & MEM_COMPACT_PURGE_HANDLES;

  while(heap != 0)
    {
      if(target_options != 0)
        {
          memory_type = heap->memory_type;
          if((memory_type & MEMTYPE_VRAM) != 0)
            {
              if(target_vram == 0)
                {
                  if((memory_type & MEMTYPE_DRAM) == 0 ||
                     target_dram == 0)
                    {
                      heap = heap->next;
                      continue;
                    }
                }
            }
          else if((memory_type & MEMTYPE_DRAM) == 0 ||
                  target_dram == 0)
            {
              heap = heap->next;
              continue;
            }
        }

      if(purge_handles != 0)
        {
          PurgeAllBlksInBank(heap);
        }

      if(CompactBank(heap, MEM_BLOCK_KIND_HANDLE) != 0)
        {
          return 0;
        }

      heap = heap->next;
    }

  if(memory_allocator_globals.memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return MEM_GetLargestBlockSize(MEMTYPE_ANY);
}

