#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





void *
MEM_NewPointerAsync(int                     byte_count,
                    uint32                  memory_type,
                    uint32                  options,
                    MemoryPressureMonitor **monitor_ref)
{
  uint32 memtype_flags;
  void *pointer;

  memtype_flags = memory_request_to_memtype_flags(memory_type);
  options |= MEM_POINTER_OPTION_POINTER_BLOCK;
  pointer = MEM_NewPointer(byte_count, memtype_flags, options);
  if(pointer != 0)
    {
      return pointer;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  record_memory_pressure_request(byte_count, memtype_flags, options,
                                 monitor_ref);
  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}
