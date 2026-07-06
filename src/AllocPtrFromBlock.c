#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void
update_largest_free_block(MemoryHeapDescriptor *heap);

MemoryBlockHeader *
AllocPtrFromBlock(MemoryBlockHeader *free_block,
                  int                block_byte_count,
                  int                allocation_kind)
{
  MemoryHeapDescriptor *heap;
  MemoryBlockHeader *allocation;
  MemoryBlockHeader *previous;
  MemoryBlockHeader *next;
  uint32 free_block_size;
  uint32 leftover_byte_count;

  free_block_size = free_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  if(free_block_size < (uint32)block_byte_count)
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return 0;
    }

  allocation = free_block;
  heap = find_memory_bank_containing_address(
    (unsigned int)free_block);
  leftover_byte_count = free_block_size - (uint32)block_byte_count;

  if(leftover_byte_count < sizeof(MemoryBlockHeader))
    {
      previous = free_block->previous;
      next = free_block->next;
      if(previous == 0)
        {
          heap->first_free_block = next;
        }
      else
        {
          previous->next = next;
        }

      if(next == 0)
        {
          heap->last_free_block = previous;
        }
      else
        {
          next->previous = previous;
        }
      block_byte_count = (int)free_block_size;
    }
  else if(allocation_kind == MEM_BLOCK_KIND_HANDLE)
    {
      link_free_memory_block(
        free_block, heap, leftover_byte_count,
        free_block->previous, free_block->next);
      allocation = (MemoryBlockHeader *)(
        (char *)free_block +
        (free_block->size_and_flags & MEM_BLOCK_SIZE_MASK));
    }
  else
    {
      link_free_memory_block(
        (MemoryBlockHeader *)((char *)free_block + block_byte_count),
        heap, leftover_byte_count,
        free_block->previous, free_block->next);
    }

  heap->free_bytes -= (uint32)block_byte_count;
  allocation->owner = 0;
  allocation->size_and_flags =
    (uint32)block_byte_count & MEM_BLOCK_SIZE_MASK;
  update_largest_free_block(heap);
  if((heap->memory_type & MEM_HEAP_VRAM_FLAG) != 0)
    {
      allocation->size_and_flags |= MEM_BLOCK_VRAM_FLAG;
    }
  allocation->owner = heap;
  return allocation;
}
