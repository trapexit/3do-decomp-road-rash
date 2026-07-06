#include "item.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

MemoryBlockHeader *
GetPtrFromBlock(MemoryBlockHeader *free_block,
                int                byte_count,
                uint32             placement_flags,
                int                allocation_kind)
{
  int requested_byte_count;
  int allocation_byte_count;
  MemoryBlockHeader *block;
  uint32 header;
  uint32 padding_byte_count;

  requested_byte_count = byte_count;
  allocation_byte_count =
    (byte_count + MEM_ALLOCATION_ALIGNMENT_MASK) &
    ~MEM_ALLOCATION_ALIGNMENT_MASK;
  if(allocation_byte_count < MEM_MINIMUM_PAYLOAD_SIZE)
    {
      allocation_byte_count = MEM_MINIMUM_PAYLOAD_SIZE;
    }
  allocation_byte_count += MEM_POINTER_HEADER_SIZE;

  block = AllocPtrFromBlock(
    free_block, allocation_byte_count, allocation_kind);
  if(block == 0)
    {
      return 0;
    }

  header = block->size_and_flags | MEM_BLOCK_ALLOCATED_FLAG;
  block->size_and_flags = header;
  if(placement_flags == 0)
    {
      header |= MEM_BLOCK_FIXED_HIGH_FLAG;
      block->size_and_flags = header;
    }

  header = block->size_and_flags;
  padding_byte_count =
    ((header & MEM_BLOCK_SIZE_MASK) - MEM_POINTER_HEADER_SIZE) -
    (uint32)requested_byte_count;
  header =
    (header & ~MEM_BLOCK_PADDING_MASK) |
    ((padding_byte_count << MEM_BLOCK_PADDING_SHIFT) &
     MEM_BLOCK_PADDING_MASK);
  block->size_and_flags = header;
  return block;
}
