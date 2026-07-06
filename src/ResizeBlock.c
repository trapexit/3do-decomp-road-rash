/* ResizeBlock - reconstructed from the 0x00036A28 routine. */

#include "memory_manager_runtime.h"

int
ResizeBlock(MemoryBlockHeader *block,
            int                byte_count)
{
  MemoryBlockHeader *split_block;
  MemoryHeapDescriptor *heap;
  uint32 header;
  uint32 block_byte_count;
  uint32 payload_byte_count;
  uint32 new_block_byte_count;
  uint32 total_slack;

  if(byte_count < 0)
    {
      return 0;
    }
  header = block->size_and_flags;
  block_byte_count = header & MEM_BLOCK_SIZE_MASK;
  payload_byte_count = block_byte_count - MEM_POINTER_HEADER_SIZE -
    ((header & MEM_BLOCK_PADDING_MASK) >> MEM_BLOCK_PADDING_SHIFT);
  if((uint32)byte_count > payload_byte_count)
    {
      return 0;
    }
  if((uint32)byte_count == payload_byte_count)
    {
      return 1;
    }

  new_block_byte_count =
    ((uint32)byte_count + MEM_ALLOCATION_ALIGNMENT_MASK) &
    ~MEM_ALLOCATION_ALIGNMENT_MASK;
  if(new_block_byte_count < MEM_MINIMUM_PAYLOAD_SIZE)
    {
      new_block_byte_count = MEM_MINIMUM_PAYLOAD_SIZE;
    }
  new_block_byte_count += MEM_POINTER_HEADER_SIZE;
  total_slack = block_byte_count - new_block_byte_count;
  if(total_slack < MEM_SPLIT_HEADER_OVERHEAD)
    {
      new_block_byte_count = block_byte_count;
    }
  block->size_and_flags =
    (header & (MEM_BLOCK_FLAGS_MASK & ~MEM_BLOCK_PADDING_MASK)) |
    new_block_byte_count |
    (((new_block_byte_count - MEM_POINTER_HEADER_SIZE -
       (uint32)byte_count) << MEM_BLOCK_PADDING_SHIFT) &
     MEM_BLOCK_PADDING_MASK);

  if(total_slack >= MEM_SPLIT_HEADER_OVERHEAD)
    {
      split_block = (MemoryBlockHeader *)
                    ((char *)block + new_block_byte_count);
      split_block->size_and_flags = total_slack;
      heap = find_memory_bank_containing_address((uint32)block);
      heap->free_bytes += total_slack;
      InsertNewFreeBlk(heap, split_block);
    }
  return 1;
}
