#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
MEM_ValidateHandle(MemoryHandle *handle)
{
  MemoryHandle **back_pointer;

  if(handle == 0 || handle->data == 0 ||
     (handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      memory_system_last_error = MEM_ERROR_BAD_HANDLE;
      return MEM_ERROR_BAD_HANDLE;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  back_pointer = (MemoryHandle **)((char *)handle->data -
                                   MEM_HANDLE_BACK_POINTER_SIZE);
  if(*back_pointer == handle)
    {
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  memory_system_last_error = MEM_ERROR_BAD_HANDLE;
  UnlockItem(memory_allocator_globals.lock_item);
  return MEM_ERROR_BAD_HANDLE;
}
