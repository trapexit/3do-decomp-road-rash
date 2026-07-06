#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

MemoryHandle *
ReturnHandleToFreeList(MemoryHandle *handle)
{
  handle->data = memory_allocator_globals.linked_list_head;
  handle->flags = 0;
  memory_allocator_globals.linked_list_head = handle;
  return handle;
}
