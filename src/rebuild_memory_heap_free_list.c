/* Original address: 0x00037248. */

#include "memory_manager_runtime.h"

void
rebuild_memory_heap_free_list(MemoryHeapDescriptor *heap)
{
  MemoryBlockHeader *previous_free_block;
  MemoryBlockHeader *first_free_block;
  MemoryBlockHeader *largest_free_block;
  MemoryBlockHeader *block;
  unsigned char *heap_end;
  uint32 header;
  uint32 block_size;
  uint32 largest_size;
  uint32 total_free;
  volatile uint32 scanned_block_count;

  previous_free_block = 0;
  first_free_block = 0;
  largest_free_block = 0;
  largest_size = 0;
  total_free = 0;
  scanned_block_count = 0;

  heap_end = (unsigned char *)heap->base_address + heap->size;
  block = (MemoryBlockHeader *)heap->base_address;
  while((unsigned char *)block < heap_end)
    {
      /* Retail retains this per-block counter on the stack.  It is not
       * published, but its accesses are observable because allocator
       * compaction runs against the title's audio-clock scheduling. */
      scanned_block_count++;
      header = block->size_and_flags;
      block_size = header & MEM_BLOCK_SIZE_MASK;

      if((header & MEM_BLOCK_ALLOCATED_FLAG) == 0)
        {
          if(block_size > largest_size)
            {
              largest_size = block_size;
              largest_free_block = block;
            }
          total_free += block_size;

          if(previous_free_block != 0)
            {
              previous_free_block->next = block;
              block->previous = previous_free_block;
            }
          else
            {
              first_free_block = block;
              block->previous = 0;
            }
          block->next = 0;
          previous_free_block = block;
        }

      block_size = block->size_and_flags & MEM_BLOCK_SIZE_MASK;
      block = (MemoryBlockHeader *)((char *)block + block_size);
    }

  heap->first_free_block = first_free_block;
  heap->last_free_block = previous_free_block;
  heap->largest_free_block = largest_free_block;
  heap->free_bytes = total_free;
}
