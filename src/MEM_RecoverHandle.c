#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





int
MEM_ValidateHandle(MemoryHandle *handle);

MemoryHandle *
MEM_RecoverHandle(MemoryHandle *handle)
{
  MemoryHandle *previous;
  MemoryHandle *current;

  if(handle == 0)
    {
      return 0;
    }
  if((handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return 0;
    }
  if(handle->data == 0)
    {
      return 0;
    }
  if(MEM_ValidateHandle(handle) != 0)
    {
      return 0;
    }

  if((handle->flags & MEM_HANDLE_FLAG_RELEASED) == 0)
    {
      return handle;
    }

  LockItem(memory_allocator_globals.lock_item, 1);

  current = (MemoryHandle *)memory_allocator_globals.block_list_head;
  if(current != 0)
    {
      previous = 0;
      while(current != 0 && current != handle)
        {
          previous = current;
          current = GetNextPurgeNode(current);
        }

      if(current == handle)
        {
          SkipPurgeNode(previous, current);
          UnlockItem(memory_allocator_globals.lock_item);
          handle->flags =
            (handle->flags & MEM_HANDLE_STATE_PRESERVE_MASK) |
            MEM_HANDLE_FLAG_VALID;
          return handle;
        }
    }

  memory_system_last_error = MEM_ERROR_RECOVER_FAILED;
  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}
