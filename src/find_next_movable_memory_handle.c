#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

/* Original address: 0x00036D84. */

static
int
memory_handle_is_movable_in_heap(const MemoryHandle         *handle,
                                 const MemoryHeapDescriptor *heap)
{
  uint32 payload_address;
  uint32 heap_start;
  uint32 heap_end;

  if((handle->flags & MEM_HANDLE_FLAG_BUSY) != 0 ||
     (handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return 0;
    }

  payload_address = (uint32)handle->data;
  heap_start = (uint32)heap->base_address;
  heap_end = heap_start + heap->size;
  return heap_start <= payload_address && payload_address <= heap_end;
}


MemoryHandle *
find_next_movable_memory_handle(MemoryHeapDescriptor *heap,
                                MemoryHandle         *cursor,
                                MemoryHandleBank    **bank_state,
                                MemoryHandle        **first_handle,
                                MemoryHandle        **last_handle,
                                int                   block_kind)
{
  MemoryHandleBank *bank;
  MemoryHandle *candidate;

  bank = *bank_state;
  if(cursor == 0)
    {
      bank = (MemoryHandleBank *)memory_allocator_globals.bank_pointer;
      if(bank == 0)
        {
          return 0;
        }

      if(block_kind == MEM_BLOCK_KIND_HANDLE)
        {
          while(bank->next != 0)
            {
              bank = bank->next;
            }
          *last_handle = get_memory_bank_handle_scan_bounds(
            bank, first_handle);
          cursor = *last_handle + 1;
        }
      else
        {
          *last_handle = get_memory_bank_handle_scan_bounds(
            bank, first_handle);
          cursor = *first_handle;
        }
    }

  while(bank != 0)
    {
      if(block_kind == MEM_BLOCK_KIND_HANDLE)
        {
          candidate = cursor - 1;
          if((uint32)candidate < (uint32) * first_handle)
            {
              bank = bank->previous;
              if(bank == 0)
                {
                  break;
                }
              *last_handle = get_memory_bank_handle_scan_bounds(
                bank, first_handle);
              cursor = *last_handle + 1;
              continue;
            }
        }
      else
        {
          candidate = cursor + 1;
          if((uint32)candidate > (uint32) * last_handle)
            {
              bank = bank->next;
              if(bank == 0)
                {
                  break;
                }
              *last_handle = get_memory_bank_handle_scan_bounds(
                bank, first_handle);
              cursor = *first_handle;
              continue;
            }
        }

      if(memory_handle_is_movable_in_heap(candidate, heap))
        {
          *bank_state = bank;
          return candidate;
        }
      cursor = candidate;
    }

  *bank_state = bank;
  return 0;
}
