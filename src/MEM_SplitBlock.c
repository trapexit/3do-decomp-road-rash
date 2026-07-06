#include "memory_manager_runtime.h"

void *
MEM_SplitBlock(void *pointer,
               int   byte_count)
{
  MemoryBlockHeader *block;
  MemoryBlockHeader *split_block;
  uint32 original_block_byte_count;
  uint32 original_flags;
  uint32 allocation_byte_count;
  uint32 new_block_byte_count;
  uint32 new_header;
  uint32 split_header;
  uint32 padding_flags;

  if(pointer == 0 || byte_count < 0 ||
     (uint32)byte_count >
     MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
     MEM_ALLOCATION_ALIGNMENT_MASK)
    {
      return 0;
    }

  block = (MemoryBlockHeader *)((char *)pointer -
                                MEM_POINTER_HEADER_SIZE);
  allocation_byte_count =
    ((uint32)byte_count + MEM_ALLOCATION_ALIGNMENT_MASK) &
    ~MEM_ALLOCATION_ALIGNMENT_MASK;
  original_flags = block->size_and_flags & MEM_BLOCK_FLAGS_MASK;
  original_block_byte_count =
    block->size_and_flags & MEM_BLOCK_SIZE_MASK;

  if(original_block_byte_count <
     allocation_byte_count + MEM_POINTER_HEADER_SIZE)
    {
      return 0;
    }
  if(allocation_byte_count < MEM_MINIMUM_PAYLOAD_SIZE)
    {
      return pointer;
    }
  if(original_block_byte_count < allocation_byte_count +
     MEM_SPLIT_HEADER_OVERHEAD + MEM_MINIMUM_PAYLOAD_SIZE)
    {
      return 0;
    }

  new_block_byte_count =
    allocation_byte_count + MEM_POINTER_HEADER_SIZE;
  new_header = original_flags | new_block_byte_count;
  padding_flags =
    ((allocation_byte_count - (uint32)byte_count) <<
      MEM_BLOCK_PADDING_SHIFT) & MEM_BLOCK_PADDING_MASK;
  new_header =
    (new_header & ~MEM_BLOCK_PADDING_MASK) | padding_flags;
  block->size_and_flags = new_header;

  split_block = (MemoryBlockHeader *)
                ((char *)block + (new_header & MEM_BLOCK_SIZE_MASK));
  split_block->size_and_flags = 0;
  split_block->owner = find_memory_bank_containing_address(
    (uint32)split_block);

  split_header =
    (original_block_byte_count - new_block_byte_count) &
    MEM_BLOCK_SIZE_MASK;
  split_header |= split_block->size_and_flags & MEM_BLOCK_FLAGS_MASK;
  split_header |= MEM_BLOCK_ALLOCATED_FLAG;
  if((block->size_and_flags & MEM_BLOCK_FIXED_LOW_FLAG) != 0)
    {
      split_header |= MEM_BLOCK_FIXED_LOW_FLAG;
    }
  if((block->size_and_flags & MEM_BLOCK_FIXED_HIGH_FLAG) != 0)
    {
      split_header |= MEM_BLOCK_FIXED_HIGH_FLAG;
    }
  split_block->size_and_flags = split_header;

  return (char *)split_block + MEM_POINTER_HEADER_SIZE;
}
