#include "memory_manager_runtime.h"

MemoryBlockHeader *
link_free_memory_block(MemoryBlockHeader    *block,
                       MemoryHeapDescriptor *heap,
                       uint32                size,
                       MemoryBlockHeader    *previous,
                       MemoryBlockHeader    *next)
{
  size &= MEM_BLOCK_SIZE_MASK;
  block->size_and_flags = size;
  if((heap->memory_type & MEMTYPE_VRAM) != 0)
    {
      block->size_and_flags = size | MEM_BLOCK_VRAM_FLAG;
    }

  block->owner = heap;
  block->previous = previous;
  block->next = next;

  if(next != 0)
    {
      next->previous = block;
    }
  else
    {
      heap->last_free_block = block;
    }

  if(previous != 0)
    {
      previous->next = block;
    }
  else
    {
      heap->first_free_block = block;
    }
  return block;
}
