#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
DisposePointer(void *pointer)
{
  MemoryBlockHeader *block;
  MemoryHeapDescriptor *heap;
  uint32 block_flags;

  block = (MemoryBlockHeader *)((char *)pointer -
                                MEM_POINTER_HEADER_SIZE);
  heap = find_memory_bank_containing_address((uint32)block);
  if(heap == 0 || block->owner != heap ||
     (block->size_and_flags & MEM_BLOCK_ALLOCATED_FLAG) == 0 ||
     (block->size_and_flags & MEM_BLOCK_HANDLE_OWNER_FLAG) != 0)
    {
      memory_system_last_error = MEM_ERROR_BAD_BANK;
      return MEM_ERROR_BAD_BANK;
    }

  block_flags = block->size_and_flags;
  block->size_and_flags = block_flags & ~MEM_BLOCK_ALLOCATED_FLAG;
  heap->free_bytes += block_flags & MEM_BLOCK_SIZE_MASK;
  InsertNewFreeBlk(heap, block);
  return 0;
}
