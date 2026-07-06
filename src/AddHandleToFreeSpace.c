#include "memory_manager_runtime.h"

MemoryBlockHeader *
AddHandleToFreeSpace(MemoryHandle         *handle,
                     MemoryHeapDescriptor *heap)
{
  MemoryBlockHeader *block;

  block = (MemoryBlockHeader *)(
    (char *)handle->data - MEM_POINTER_HEADER_SIZE);
  block->size_and_flags &= ~MEM_BLOCK_ALLOCATED_FLAG;
  heap->free_bytes += block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  InsertNewFreeBlk(heap, block);
  handle->data = 0;
  handle->flags = 0;
  return block;
}
