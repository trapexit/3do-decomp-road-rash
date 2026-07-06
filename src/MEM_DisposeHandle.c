#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





int
MEM_DisposeHandle(MemoryHandle *handle)
{
  MemoryHeapDescriptor *memory_bank;
  MemoryHandle *node;
  MemoryHandle *previous;
  MemoryHandle *next;

  if(handle == 0)
    {
      return MEM_ERROR_INVALID_HANDLE;
    }

  LockItem(memory_allocator_globals.lock_item, 1);

  if(handle->data == 0)
    {
      node = (MemoryHandle *)memory_allocator_globals.linked_list_head;
      while(node != 0)
        {
          if(node == handle)
            {
              UnlockItem(memory_allocator_globals.lock_item);
              return MEM_ERROR_INVALID_HANDLE;
            }
          node = (MemoryHandle *)node->data;
        }
      ReturnHandleToFreeList(handle);
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  if((handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      UnlockItem(memory_allocator_globals.lock_item);
      return MEM_ERROR_INVALID_HANDLE;
    }


  memory_bank = find_memory_bank_containing_address(
    (uint32)handle->data - MEM_POINTER_HEADER_SIZE);

  if((handle->flags & MEM_HANDLE_FLAG_RELEASED) != 0)
    {
      previous = 0;
      next = (MemoryHandle *)memory_allocator_globals.block_list_head;
      for(;;)
        {
          node = next;
          if(node == 0)
            {
              UnlockItem(memory_allocator_globals.lock_item);
              return MEM_ERROR_HANDLE_NOT_QUEUED;
            }
          if(node == handle)
            {
              SkipPurgeNode(previous, node);
              break;
            }
          next = GetNextPurgeNode(node);
          previous = node;
        }
    }

  AddHandleToFreeSpace(handle, memory_bank);
  ReturnHandleToFreeList(handle);

  if(memory_allocator_globals.memory_pressure_monitor_head_ref != 0)
    {
      update_memory_pressure_monitor();
    }

  UnlockItem(memory_allocator_globals.lock_item);
  return 0;
}
