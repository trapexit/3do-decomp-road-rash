#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





MemoryBlockHeader *
FindFreeBlockFor(MemoryHeapDescriptor *heap,
                 int                   byte_count,
                 uint32                options,
                 int                   block_kind)
{
  MemoryBlockHeader *block;
  uint32 required_byte_count;

  if(heap == 0)
    {
      return 0;
    }

  if((options & MEM_POINTER_OPTION_COMPACT_FIRST) != 0 &&
     CompactBank(heap, block_kind) != 0)
    {
      return 0;
    }

  required_byte_count =
    (((uint32)byte_count + MEM_ALLOCATION_ALIGNMENT_MASK) &
     ~MEM_ALLOCATION_ALIGNMENT_MASK) + MEM_POINTER_HEADER_SIZE;
  block = heap->largest_free_block;
  if(block != 0 &&
     required_byte_count <=
     (block->size_and_flags & MEM_BLOCK_SIZE_MASK))
    {
      return block;
    }

  if(required_byte_count <= heap->free_bytes)
    {
      if((options & MEM_POINTER_OPTION_ALLOW_FAILURE) != 0)
        {
          memory_system_last_error = MEM_ERROR_FRAGMENTED_FREE_SPACE;
          return 0;
        }
      if(CompactBank(heap, block_kind) != 0)
        {
          return 0;
        }
    }

  block = heap->largest_free_block;
  if(block != 0 &&
     required_byte_count <=
     (block->size_and_flags & MEM_BLOCK_SIZE_MASK))
    {
      return block;
    }
  return 0;
}
