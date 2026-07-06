#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





MemoryHandle *
MEM_NewHandle(int    byte_count,
              uint32 memory_type,
              uint32 options)
{
  MemoryAllocatorGlobals *state;
  uint32 memtype_flags;
  MemoryHandle *handle;

  state = &memory_allocator_globals;

  LockItem(state->lock_item, 1);

  memtype_flags = memory_request_to_memtype_flags(memory_type);
  handle = NewHandle(byte_count, memtype_flags, options);

  if(state->memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }

  UnlockItem(state->lock_item);
  return handle;
}
