#include "memory_manager_runtime.h"

MemoryBlockHeader *
InsertNewFreeBlk(MemoryHeapDescriptor *heap,
                 MemoryBlockHeader    *block)
{
  MemoryBlockHeader *previous_block;
  MemoryBlockHeader *next_block;
  MemoryBlockHeader *result;
  unsigned int block_size;

  previous_block = 0;
  next_block = heap->first_free_block;
  while(next_block != 0 &&
        (unsigned int)block > (unsigned int)next_block)
    {
      previous_block = next_block;
      next_block = next_block->next;
    }

  result = link_free_memory_block(
    block, heap, block->size_and_flags & MEM_BLOCK_SIZE_MASK,
    previous_block, next_block);
  if(heap->largest_free_block == 0 ||
     (heap->largest_free_block->size_and_flags & MEM_BLOCK_SIZE_MASK) <
     (block->size_and_flags & MEM_BLOCK_SIZE_MASK))
    {
      heap->largest_free_block = block;
    }

  if(previous_block != 0)
    {
      block_size =
        previous_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
      if((MemoryBlockHeader *)((char *)previous_block + block_size) ==
         block)
        {
          result = merge_adjacent_free_memory_blocks(
            heap, previous_block, block);
          block = result;
        }
    }

  if(next_block == 0)
    {
      return result;
    }

  if(block == 0)
    {
      result = 0;
    }
  else
    {
      block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
      result = (MemoryBlockHeader *)((char *)block + block_size);
    }
  if(result != next_block)
    {
      return result;
    }

  return merge_adjacent_free_memory_blocks(heap, block, next_block);
}
