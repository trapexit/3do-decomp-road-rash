#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





void *
MEM_NewPointer(int    byte_count,
               uint32 memory_type,
               uint32 options)
{
  uint32 memtype_flags;
  void *pointer;

  LockItem(memory_allocator_globals.lock_item, 1);
  memtype_flags = memory_request_to_memtype_flags(memory_type);
  pointer = NewPointer(byte_count, memtype_flags, options);
  if(memory_allocator_globals.memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }
  UnlockItem(memory_allocator_globals.lock_item);

  return pointer;
}
