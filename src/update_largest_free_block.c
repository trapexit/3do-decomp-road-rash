/* Reconstructed from the 0x00035D00 largest-free-block scan. */

#include "memory_manager_runtime.h"

void
update_largest_free_block(MemoryHeapDescriptor *heap)
{
  MemoryBlockHeader *block;
  MemoryBlockHeader *largest_block;
  uint32 largest_size;
  uint32 block_size;

  largest_size = 0;
  largest_block = 0;
  block = heap->first_free_block;
  while(block != 0)
    {
      block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
      if(largest_size <= block_size)
        {
          largest_block = block;
          largest_size = block_size;
        }
      block = block->next;
    }
  heap->largest_free_block = largest_block;
}
