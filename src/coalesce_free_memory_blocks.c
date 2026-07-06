#include "memory_manager_runtime.h"

void
coalesce_free_memory_blocks(MemoryHeapDescriptor *heap)
{
  MemoryBlockHeader *block;
  MemoryBlockHeader *next_block;
  unsigned int block_size;

  block = heap->first_free_block;
  while(block != 0)
    {
      next_block = block->next;
      if(next_block == 0)
        {
          break;
        }

      block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
      if((MemoryBlockHeader *)((char *)block + block_size) ==
         next_block)
        {
          block = merge_adjacent_free_memory_blocks(
            heap, block, next_block);
        }
      else
        {
          block = next_block;
        }
    }
}
