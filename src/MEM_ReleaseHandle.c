#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





int
MEM_ReleaseHandle(MemoryHandle *handle)
{
  uint32 flags;

  if(handle == 0 || handle->data == 0 ||
     (handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return MEM_ERROR_INVALID_HANDLE;
    }

  if((handle->flags & MEM_HANDLE_FLAG_RELEASED) != 0)
    {
      return 0;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  flags = handle->flags;
  flags &= ~MEM_HANDLE_FLAG_BUSY;
  flags |= MEM_HANDLE_FLAG_RELEASED;
  handle->flags = flags;
  AppendPurgeList(handle);

  if(memory_allocator_globals.memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}
