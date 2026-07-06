#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

MemoryHandle *
GetNextPurgeNode(MemoryHandle *handle)
{
  uint32 link_offset;
  MemoryHandle *next;

  link_offset = handle->flags & MEM_HANDLE_USER_DATA_MASK;
  if(link_offset == MEM_PURGE_LINK_END)
    {
      return 0;
    }

  if((handle->flags & MEM_PURGE_LINK_REVERSED_FLAG) == 0)
    {
      next = (MemoryHandle *)((char *)handle + link_offset);
    }
  else
    {
      next = (MemoryHandle *)((char *)handle - link_offset);
    }

  if(next == *(MemoryHandle **)(
       (char *)next->data - MEM_HANDLE_BACK_POINTER_SIZE))
    {
      return next;
    }

  memory_system_last_error = MEM_ERROR_BAD_HANDLE;
  return 0;
}
