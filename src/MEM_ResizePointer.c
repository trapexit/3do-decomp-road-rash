#include "item.h"
#include "string.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

void *
MEM_ResizePointer(void  *pointer,
                  int    byte_count,
                  uint32 options)
{
  MemoryBlockHeader *block;
  MemoryBlockHeader *new_block;
  uint32 flags;
  uint32 placement_flags;
  int copy_byte_count;
  void *new_pointer;

  if(pointer == 0)
    {
      return 0;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  block = (MemoryBlockHeader *)((char *)pointer -
                                MEM_POINTER_HEADER_SIZE);

  if(ResizeBlock(block, byte_count) != 0)
    {
      UnlockItem(memory_allocator_globals.lock_item);
      return pointer;
    }

  flags = block->size_and_flags;
  if((flags & MEM_BLOCK_FIXED_LOW_FLAG) == 0)
    {
      placement_flags = MEM_BLOCK_ALLOCATE_HIGH_FLAG;
    }
  else
    {
      placement_flags = MEM_BLOCK_ALLOCATE_LOW_FLAG;
    }
  if((flags & MEM_BLOCK_FIXED_HIGH_FLAG) != 0)
    {
      placement_flags = 0;
    }

  new_block = NewBlock(
    byte_count, placement_flags,
    options | MEM_POINTER_OPTION_POINTER_BLOCK);
  if(new_block == 0)
    {
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  copy_byte_count =
    (block->size_and_flags & MEM_BLOCK_SIZE_MASK) -
    MEM_POINTER_HEADER_SIZE;
  if((uint32)copy_byte_count >= (uint32)byte_count)
    {
      copy_byte_count = byte_count;
    }

  new_pointer = (char *)new_block + MEM_POINTER_HEADER_SIZE;
  memcpy(new_pointer, pointer, copy_byte_count);
  DisposePointer(pointer);
  UnlockItem(memory_allocator_globals.lock_item);
  return new_pointer;
}
