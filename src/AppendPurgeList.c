
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void
AppendPurgeList(MemoryHandle *handle)
{
  MemoryHandle *node;
  MemoryBlockHeader *block;
  MemoryHeapDescriptor *heap;
  int link_delta;
  uint32 flags;
  uint32 block_size;

  node = (MemoryHandle *)memory_allocator_globals.block_list_head;
  if(node == 0)
    {
      memory_allocator_globals.block_list_head = handle;
    }
  else
    {
      while(GetNextPurgeNode(node) != 0)
        {
          node = GetNextPurgeNode(node);
        }

      link_delta = (int)handle - (int)node;
      flags = node->flags;
      if(link_delta >= 0)
        {
          flags &= ~MEM_PURGE_LINK_REVERSED_FLAG;
        }
      else
        {
          link_delta = -link_delta;
          flags |= MEM_PURGE_LINK_REVERSED_FLAG;
        }
      node->flags =
        (flags & MEM_HANDLE_FLAG_MASK) |
        ((uint32)link_delta & MEM_HANDLE_USER_DATA_MASK);
    }

  handle->flags |= MEM_PURGE_LINK_END;
  block = (MemoryBlockHeader *)(
    (char *)handle->data - MEM_POINTER_HEADER_SIZE);
  heap = find_memory_bank_containing_address((uint32)block);
  block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  heap->purgeable_bytes += block_size;

  if((handle->flags & MEM_HANDLE_FLAG_SECONDARY_MEMORY) != 0)
    {
      heap->secondary_purgeable_bytes += block_size;
    }
}
