#include "string.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

static void
move_memory_to_lower_address(void       *destination,
                             const void *source,
                             uint32      byte_count)
{
  uint32 *destination_words;
  const uint32 *source_words;
  unsigned char *destination_bytes;
  const unsigned char *source_bytes;
  uint32 word_count;

  destination_words = (uint32 *)destination;
  source_words = (const uint32 *)source;
  word_count = byte_count / sizeof(uint32);
  while(word_count != 0)
    {
      *destination_words++ = *source_words++;
      word_count--;
    }

  destination_bytes = (unsigned char *)destination_words;
  source_bytes = (const unsigned char *)source_words;
  byte_count &= sizeof(uint32) - 1U;
  while(byte_count != 0)
    {
      *destination_bytes++ = *source_bytes++;
      byte_count--;
    }
}

static void
move_memory_to_higher_address(void       *destination,
                              const void *source,
                              uint32      byte_count)
{
  unsigned char *destination_bytes;
  const unsigned char *source_bytes;

  destination_bytes = (unsigned char *)destination;
  source_bytes = (const unsigned char *)source;

  while((byte_count & (sizeof(uint32) - 1U)) != 0)
    {
      byte_count--;
      destination_bytes[byte_count] = source_bytes[byte_count];
    }

  while(byte_count != 0)
    {
      byte_count -= sizeof(uint32);
      *(uint32 *)(destination_bytes + byte_count) =
        *(const uint32 *)(source_bytes + byte_count);
    }
}

int
MoveBlkToFreeSpace(MemoryHeapDescriptor *heap,
                   MemoryBlockHeader   **allocated_block_ref,
                   MemoryBlockHeader   **free_block_ref,
                   int                   move_direction)
{
  MemoryBlockHeader *allocated_block;
  MemoryBlockHeader *free_block;
  MemoryHandle *owner;
  MemoryBlockHeader *previous_free_block;
  MemoryBlockHeader *next_free_block;
  MemoryBlockHeader *relocated_block;
  MemoryBlockHeader *remaining_free_block;
  uint32 allocated_size;
  uint32 free_size;
  uint32 allocated_flags;
  int adjacent;

  allocated_block = *allocated_block_ref;
  free_block = *free_block_ref;
  allocated_size =
    allocated_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  free_size = free_block->size_and_flags & MEM_BLOCK_SIZE_MASK;
  allocated_flags =
    allocated_block->size_and_flags & MEM_BLOCK_FLAGS_MASK;
  owner = (MemoryHandle *)allocated_block->owner;

  if((MemoryBlockHeader *)((char *)owner->data -
                           MEM_POINTER_HEADER_SIZE) != allocated_block)
    {
      memory_system_last_error = MEM_ERROR_BAD_HANDLE;
      return MEM_ERROR_BAD_HANDLE;
    }

  remaining_free_block = free_block;
  previous_free_block = free_block->previous;
  next_free_block = free_block->next;
  adjacent = 0;

  if((uint32)allocated_block < (uint32)free_block)
    {
      if(move_direction == MEM_MOVE_TOWARD_LOW_ADDRESS)
        {
          return MEM_MOVE_RESULT_DIRECTION_MISMATCH;
        }
      if((MemoryBlockHeader *)((char *)allocated_block +
                               allocated_size) == free_block)
        {
          adjacent = 1;
        }
    }
  else
    {
      if(move_direction == MEM_MOVE_TOWARD_HIGH_ADDRESS)
        {
          return MEM_MOVE_RESULT_DIRECTION_MISMATCH;
        }
      if((MemoryBlockHeader *)((char *)free_block + free_size) ==
         allocated_block)
        {
          adjacent = 1;
        }
    }

  if(allocated_size <= free_size)
    {
      relocated_block = AllocPtrFromBlock(
        free_block, (int)allocated_size, move_direction);
      memcpy((char *)relocated_block + MEM_POINTER_HEADER_SIZE,
             (char *)allocated_block + MEM_POINTER_HEADER_SIZE,
             allocated_size - MEM_POINTER_HEADER_SIZE);
      InsertNewFreeBlk(heap, allocated_block);
    }
  else
    {
      if(adjacent == 0)
        {
          return MEM_MOVE_RESULT_RETRY;
        }

      if(move_direction == MEM_MOVE_TOWARD_LOW_ADDRESS)
        {
          move_memory_to_lower_address(
            free_block, allocated_block, allocated_size);
          remaining_free_block = (MemoryBlockHeader *)(
            (char *)free_block + allocated_size);
          link_free_memory_block(
            remaining_free_block, heap, free_size,
            previous_free_block, next_free_block);
          relocated_block = free_block;
        }
      else
        {
          move_memory_to_higher_address(
            (char *)allocated_block + free_size,
            allocated_block, allocated_size);
          remaining_free_block = allocated_block;
          relocated_block = (MemoryBlockHeader *)(
            (char *)allocated_block + free_size);
          link_free_memory_block(
            remaining_free_block, heap, free_size,
            previous_free_block, next_free_block);
        }
    }

  relocated_block->owner = owner;
  relocated_block->size_and_flags =
    allocated_flags | relocated_block->size_and_flags;
  owner->data = (char *)relocated_block + MEM_POINTER_HEADER_SIZE;
  *allocated_block_ref = relocated_block;
  *free_block_ref = remaining_free_block;
  return MEM_MOVE_RESULT_MOVED;
}
