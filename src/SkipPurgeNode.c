#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void
SkipPurgeNode(MemoryHandle *previous,
              MemoryHandle *handle)
{
  MemoryHandle *next;
  MemoryBlockHeader *block;
  MemoryHeapDescriptor *heap;
  int link_delta;
  uint32 flags;
  uint32 block_size;

  next = GetNextPurgeNode(handle);

  if(previous == 0)
    {
      memory_allocator_globals.block_list_head = next;
      return;
    }

  if(next == 0)
    {
      previous->flags |= MEM_PURGE_LINK_END;
      return;
    }

  link_delta = (int)next - (int)previous;
  if(link_delta < 0)
    {
      link_delta = -link_delta;
      flags = previous->flags | MEM_PURGE_LINK_REVERSED_FLAG;
    }
  else
    {
      flags = previous->flags & ~MEM_PURGE_LINK_REVERSED_FLAG;
    }

  previous->flags =
    (flags & MEM_HANDLE_FLAG_MASK) |
    ((uint32)link_delta & MEM_HANDLE_USER_DATA_MASK);

  block = (MemoryBlockHeader *)(
    (char *)handle->data - MEM_POINTER_HEADER_SIZE);
  heap = find_memory_bank_containing_address((uint32)block);

  block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  heap->purgeable_bytes -= block_size;

  if((handle->flags & MEM_HANDLE_FLAG_SECONDARY_MEMORY) != 0)
    {
      heap->secondary_purgeable_bytes -= block_size;
    }
}
