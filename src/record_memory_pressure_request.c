#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
record_memory_pressure_request(int                     byte_count,
                               uint32                  unused_memtype_flags,
                               uint32                  unused_options,
                               MemoryPressureMonitor **monitor_ref)
{
  MemoryPressureMonitor *monitor;
  int index;

  monitor = *monitor_ref;
  if(monitor->request_index >= monitor->request_capacity)
    {
      monitor->request_index = 0;
    }

  index = monitor->request_index;
  monitor->requested_bytes[index] = byte_count;
  monitor->request_index = index + 1;
  return index + 1;
}


int
register_memory_pressure_monitor(MemoryPressureMonitor **monitor_ref)
{
  MemoryPressureMonitor **current_ref;
  MemoryPressureMonitor *monitor;

  LockItem(memory_allocator_globals.lock_item, 1);

  current_ref = memory_allocator_globals.memory_pressure_monitor_head_ref;
  if(current_ref != 0)
    {
      monitor = *current_ref;
      while(monitor->next_ref != 0)
        {
          monitor = *monitor->next_ref;
        }
      monitor->next_ref = monitor_ref;
    }

  return UnlockItem(memory_allocator_globals.lock_item);
}


int
unregister_memory_pressure_monitor(MemoryPressureMonitor **monitor_ref)
{
  MemoryPressureMonitor **previous_ref;
  MemoryPressureMonitor **current_ref;
  MemoryPressureMonitor **next_ref;

  LockItem(memory_allocator_globals.lock_item, 1);

  current_ref = memory_allocator_globals.memory_pressure_monitor_head_ref;
  if(current_ref != 0)
    {
      previous_ref = 0;
      while(current_ref != 0)
        {
          if(current_ref == monitor_ref)
            {
              next_ref = (*current_ref)->next_ref;
              if(previous_ref == 0)
                {
                  memory_allocator_globals.memory_pressure_monitor_head_ref =
                    next_ref;
                }
              else
                {
                  (*previous_ref)->next_ref = next_ref;
                }
              UnlockItem(memory_allocator_globals.lock_item);
              return 0;
            }
          previous_ref = current_ref;
          current_ref = (*current_ref)->next_ref;
        }
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return MEM_ERROR_MONITOR_NOT_REGISTERED;
}
