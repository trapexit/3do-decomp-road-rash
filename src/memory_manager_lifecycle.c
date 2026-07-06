#include "platform.h"
#include "string.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
CreateMoreMasters(int memory_type,
                  int handle_count)
{
  MemoryAllocatorGlobals *state;
  MemoryHandle *handles;
  MemoryHandle *handle;
  MemoryHandleBank *bank;
  MemoryHandleBank *bank_tail;
  int index;

  (void)memory_type;

  state = &memory_allocator_globals;
  if(handle_count <= 0)
    {
      handle_count = MEM_HANDLE_POOL_INITIAL_COUNT;
    }
  if(handle_count < 2 ||
     (uint32)handle_count >
     (MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
      MEM_ALLOCATION_ALIGNMENT_MASK) / sizeof(MemoryHandle))
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return MEM_ERROR_NO_FREE_BLOCK;
    }

  handles = (MemoryHandle *)NewPointer(
    handle_count * sizeof(MemoryHandle), MEMTYPE_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(handles == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return MEM_ERROR_NO_FREE_BLOCK;
    }

  for(index = 0; index < handle_count - 1; index++)
    {
      handle = &handles[index];
      handle->flags = 0;
      handle->data = &handles[index + 1];
    }

  handle = &handles[index];
  handle->flags = 0;
  handle->data = state->linked_list_head;
  state->linked_list_head = handles;

  state->linked_list_head = handles[0].data;
  handles[0].data = 0;
  handles[0].flags = 0;

  bank = (MemoryHandleBank *)((char *)handles - MEM_BANK_HEADER_SIZE);
  bank_tail = (MemoryHandleBank *)state->bank_pointer;
  bank->previous = 0;
  if(bank_tail == 0)
    {
      state->bank_pointer = bank;
    }
  else
    {
      while(bank_tail->next != 0)
        {
          bank_tail = bank_tail->next;
        }
      bank_tail->next = bank;
      bank->previous = bank_tail;
    }

  return 0;
}


int
MEM_Close(void)
{
  MemoryAllocatorGlobals *state;
  MemoryRegionHeader *region;
  MemoryRegionHeader *next_region;
  int lock_item;

  state = &memory_allocator_globals;
  lock_item = state->lock_item;
  if(lock_item <= 0)
    {
      return 0;
    }
  if(LockItem(lock_item, 1) < 0)
    {
      return MEM_ERROR_NO_MEMORY_BANK;
    }

  region = (MemoryRegionHeader *)state->block_list_tail;
  while(region != 0)
    {
      next_region = region->next;
      FreeMemToMemLists(
        CURRENTTASK->t_FreeMemoryLists, region->base_address,
        region->size);
      region = next_region;
    }

  bzero(state, offsetof(MemoryAllocatorGlobals, primary_memory_type));
  UnlockItem(lock_item);
  DeleteItem(lock_item);
  return 0;
}
