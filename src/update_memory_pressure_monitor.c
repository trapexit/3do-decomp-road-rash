#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"
#include "task.h"

int
update_memory_pressure_monitor(void)
{
  MemoryPressureMonitor *monitor;
  MemoryHeapDescriptor *heap;
  int reclaimable_dram_bytes;
  int available_limit;
  int triggered;
  int i;
  int requested_bytes;

  if(memory_allocator_globals.memory_pressure_monitor_head_ref == 0)
    {
      return 0;
    }

  monitor = *memory_allocator_globals.memory_pressure_monitor_head_ref;
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;

  monitor->dram_free_bytes = 0;
  monitor->vram_free_bytes = 0;
  monitor->dram_purgeable_bytes = 0;
  monitor->vram_purgeable_bytes = 0;
  monitor->dram_secondary_purgeable_bytes = 0;
  monitor->vram_secondary_purgeable_bytes = 0;

  while(heap != 0)
    {
      if((heap->memory_type & MEMTYPE_DRAM) != 0)
        {
          monitor->dram_free_bytes += heap->free_bytes;
          monitor->dram_purgeable_bytes += heap->purgeable_bytes;
          monitor->dram_secondary_purgeable_bytes +=
            heap->secondary_purgeable_bytes;
        }
      else
        {
          monitor->vram_free_bytes += heap->free_bytes;
          monitor->vram_purgeable_bytes += heap->purgeable_bytes;
          monitor->vram_secondary_purgeable_bytes +=
            heap->secondary_purgeable_bytes;
        }
      heap = heap->next;
    }

  reclaimable_dram_bytes =
    monitor->dram_purgeable_bytes -
    monitor->dram_secondary_purgeable_bytes;
  if(monitor->dram_free_bytes >= monitor->vram_free_bytes)
    {
      available_limit = monitor->dram_free_bytes;
    }
  else
    {
      available_limit = monitor->vram_free_bytes;
    }

  triggered = 0;
  for(i = 0; i < monitor->request_index && triggered == 0; i++)
    {
      requested_bytes = monitor->requested_bytes[i];
      if(requested_bytes <= available_limit ||
         requested_bytes <= reclaimable_dram_bytes)
        {
          triggered = 1;
        }
    }

  if(triggered == 0)
    {
      return (int)monitor;
    }
  if(monitor->task_item == 0)
    {
      return (int)monitor;
    }
  if(monitor->signal_mask == 0)
    {
      return (int)&monitor->signal_mask;
    }

  return SendSignal(monitor->task_item, monitor->signal_mask);
}
