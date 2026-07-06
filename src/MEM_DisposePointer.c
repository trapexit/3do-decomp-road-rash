#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
MEM_DisposePointer(void *pointer)
{
  int result;

  if(pointer == 0)
    {
      return MEM_ERROR_BAD_ADDRESS;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  result = DisposePointer(pointer);
  if(memory_allocator_globals.memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }
  UnlockItem(memory_allocator_globals.lock_item);

  return result;
}
