#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





MemoryHandle *
GetNewMasterPtr(MemoryBlockHeader *block)
{
  MemoryHandle *handle;

  if(memory_allocator_globals.linked_list_head == 0 &&
     CreateMoreMasters(0, MEM_HANDLE_POOL_INITIAL_COUNT) != 0)
    {
      return 0;
    }

  handle = (MemoryHandle *)memory_allocator_globals.linked_list_head;
  memory_allocator_globals.linked_list_head = handle->data;
  handle->data = (char *)block + MEM_POINTER_HEADER_SIZE;
  handle->flags = MEM_HANDLE_FLAG_VALID;
  return handle;
}
