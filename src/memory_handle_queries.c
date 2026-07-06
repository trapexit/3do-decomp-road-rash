#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
MEM_GetLargestBlockSize(uint32 memory_request)
{
  MemoryHeapDescriptor *heap;
  MemoryBlockHeader *block;
  uint32 memory_type_flags;
  int largest_size;
  int block_size;

  largest_size = 0;
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  memory_type_flags = memory_request_to_memtype_flags(memory_request);

  while(heap != 0)
    {
      if((heap->memory_type & memory_type_flags) != 0 ||
         memory_type_flags == 0)
        {
          block = heap->largest_free_block;
          if(block != 0)
            {
              block_size =
                (int)(block->size_and_flags & MEM_BLOCK_SIZE_MASK);
              if(largest_size < block_size)
                {
                  largest_size = block_size;
                }
            }
        }
      heap = heap->next;
    }

  return largest_size;
}


void *
MEM_LockHandle(MemoryHandle *handle)
{
  handle->flags |= MEM_HANDLE_FLAG_BUSY;
  return handle->data;
}


int
MEM_UnlockHandle(MemoryHandle *handle)
{
  handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  return 0;
}


int
MEM_IsHandleLocked(const MemoryHandle *handle)
{
  if(handle == 0 || handle->data == 0)
    {
      return 0;
    }
  if((handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return 0;
    }
  if((handle->flags & MEM_HANDLE_FLAG_BUSY) == 0)
    {
      return 0;
    }
  return 1;
}


void
MEM_SetHandleLocked(MemoryHandle *handle,
                    int           locked)
{
  if(handle == 0 || handle->data == 0)
    {
      return;
    }
  if((handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return;
    }

  if(((uint32)locked & MEM_HANDLE_LOCK_VALUE_MASK) == 0)
    {
      MEM_UnlockHandle(handle);
      return;
    }
  handle->flags |= MEM_HANDLE_FLAG_BUSY;
}


static
int
memory_block_payload_size(const MemoryBlockHeader *header)
{
  uint32 header_word;
  int block_size;
  int padding_size;

  header_word = header->size_and_flags;
  block_size = (int)(header_word & MEM_BLOCK_SIZE_MASK);
  padding_size = (int)((header_word & MEM_BLOCK_PADDING_MASK) >>
                       MEM_BLOCK_PADDING_SHIFT);
  return block_size - padding_size - MEM_POINTER_HEADER_SIZE;
}


int
MEM_GetHandleSize(MemoryHandle *handle)
{
  MemoryBlockHeader *header;

  if(handle != 0 && handle->data != 0 &&
     (handle->flags & MEM_HANDLE_FLAG_VALID) != 0)
    {
      LockItem(memory_allocator_globals.lock_item, 1);
      header = (MemoryBlockHeader *)((char *)handle->data -
                                     MEM_POINTER_HEADER_SIZE);
      UnlockItem(memory_allocator_globals.lock_item);
      return memory_block_payload_size(header);
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}


int
MEM_GetPointerSize(void *pointer)
{
  MemoryBlockHeader *header;

  if(pointer != 0)
    {
      LockItem(memory_allocator_globals.lock_item, 1);
      header = (MemoryBlockHeader *)((char *)pointer -
                                     MEM_POINTER_HEADER_SIZE);
      UnlockItem(memory_allocator_globals.lock_item);
      return memory_block_payload_size(header);
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}


int
MEM_GetHandleUserData(const MemoryHandle *handle)
{
  if(handle != 0 && handle->data != 0 &&
     (handle->flags & MEM_HANDLE_FLAG_VALID) != 0)
    {
      return (int)(handle->flags & MEM_HANDLE_USER_DATA_MASK);
    }
  return -1;
}


int
MEM_SetHandleUserData(MemoryHandle *handle,
                      int           user_data)
{
  uint32 flags;

  if(handle != 0 && handle->data != 0 &&
     (handle->flags & MEM_HANDLE_FLAG_VALID) != 0)
    {
      flags = handle->flags;
      if((flags & MEM_HANDLE_FLAG_RELEASED) != 0)
        {
          return MEM_ERROR_BAD_HANDLE;
        }
      handle->flags = ((uint32)user_data & MEM_HANDLE_USER_DATA_MASK) |
                      (flags & MEM_HANDLE_FLAG_MASK);
      return 0;
    }
  return MEM_ERROR_INVALID_HANDLE;
}



int
MEM_ValidateAllHandles(void)
{
  MemoryHandleBank *bank;
  MemoryHandle *cursor;
  MemoryHandle *limit;

  bank = (MemoryHandleBank *)memory_allocator_globals.bank_pointer;
  for(;;)
    {
      limit = get_memory_bank_handle_scan_bounds(bank, &cursor);
      while(bank != 0)
        {
          if((cursor->flags & MEM_HANDLE_FLAG_VALID) != 0)
            {
              MEM_ValidateHandle(cursor);
            }

          cursor++;
          if((uint32)cursor > (uint32)limit)
            {
              bank = bank->next;
              if(bank == 0)
                {
                  return memory_system_last_error;
                }
              break;
            }
        }
    }
}
