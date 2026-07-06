#include "memory_manager_runtime.h"

MemoryBlockHeader *
merge_adjacent_free_memory_blocks(MemoryHeapDescriptor *heap,
                                  MemoryBlockHeader    *left_block,
                                  MemoryBlockHeader    *right_block)
{
  MemoryBlockHeader *neighbor;
  MemoryBlockHeader *largest_block;
  unsigned int left_size;
  unsigned int right_size;

  left_size = 0;
  if(left_block != 0)
    {
      left_size = left_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
    }
  right_size = 0;
  if(right_block != 0)
    {
      right_size = right_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
    }

  left_block->size_and_flags =
    ((left_size + right_size) & MEM_BLOCK_SIZE_MASK) |
    (left_block->size_and_flags & MEM_BLOCK_FLAGS_MASK);

  neighbor = right_block->previous;
  if(neighbor == 0)
    {
      heap->first_free_block = right_block->next;
    }
  else
    {
      neighbor->next = right_block->next;
    }

  neighbor = right_block->next;
  if(neighbor == 0)
    {
      heap->last_free_block = left_block;
    }
  else
    {
      neighbor->previous = left_block;
    }

  left_block->owner = heap;
  largest_block = heap->largest_free_block;
  if(largest_block != 0)
    {
      left_size = 0;
      if(left_block != 0)
        {
          left_size = left_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
        }
      if(left_size <=
         (largest_block->size_and_flags & MEM_BLOCK_SIZE_MASK))
        {
          return left_block;
        }
    }

  heap->largest_free_block = left_block;
  return left_block;
}
