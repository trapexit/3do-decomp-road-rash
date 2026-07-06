#include "item.h"
#include "string.h"
#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
MEM_ResizeHandle(MemoryHandle *handle,
                 int           byte_count,
                 uint32        options)
{
  MemoryBlockHeader *old_block;
  MemoryBlockHeader *new_block;
  uint32 old_flags;
  uint32 placement_flags;
  uint32 old_payload_size;
  uint32 copy_size;
  MemoryHeapDescriptor *heap;

  if(handle == 0 || handle->data == 0 ||
     (handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return MEM_ERROR_BAD_HANDLE;
    }

  LockItem(memory_allocator_globals.lock_item, 1);

  old_block = (MemoryBlockHeader *)((char *)handle->data -
                                    MEM_POINTER_HEADER_SIZE);
  if(ResizeBlock(old_block, byte_count) == 0)
    {
      old_flags = old_block->size_and_flags;
      if((old_flags & MEM_BLOCK_FIXED_LOW_FLAG) == 0)
        {
          placement_flags = MEM_BLOCK_ALLOCATE_HIGH_FLAG;
        }
      else
        {
          placement_flags = MEM_BLOCK_ALLOCATE_LOW_FLAG;
        }
      if((old_flags & MEM_BLOCK_FIXED_HIGH_FLAG) != 0)
        {
          placement_flags = 0;
        }

      new_block = NewBlock(byte_count, placement_flags, options);
      if(new_block == 0)
        {
          UnlockItem(memory_allocator_globals.lock_item);
          return memory_system_last_error;
        }

      old_block = (MemoryBlockHeader *)((char *)handle->data -
                                        MEM_POINTER_HEADER_SIZE);
      old_payload_size =
        (old_block->size_and_flags & MEM_BLOCK_SIZE_MASK) -
        MEM_POINTER_HEADER_SIZE;
      if(old_payload_size >= (uint32)byte_count)
        {
          copy_size = (uint32)byte_count;
        }
      else
        {
          copy_size = old_payload_size;
        }

      memcpy((char *)new_block + MEM_POINTER_HEADER_SIZE,
             handle->data, copy_size);

      heap = find_memory_bank_containing_address((uint32)old_block);
      old_block->owner = heap;
      old_block->size_and_flags &= ~MEM_BLOCK_HANDLE_OWNER_FLAG;

      DisposePointer((char *)handle->data);

      handle->data = (char *)new_block + MEM_POINTER_HEADER_SIZE;
      new_block->owner = handle;
      new_block->size_and_flags |= MEM_BLOCK_HANDLE_OWNER_FLAG;

      if((options & MEM_RESIZE_OPTION_KEEP_BUSY) == 0)
        {
          handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
        }
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}
