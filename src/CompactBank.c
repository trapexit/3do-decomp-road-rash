#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
CompactBank(MemoryHeapDescriptor *heap,
            int                   block_kind)
{
  MemoryHandleBank *bank_state;
  MemoryHandle *bank_first_handle;
  MemoryHandle *bank_last_handle;
  MemoryBlockHeader *free_block;
  MemoryBlockHeader *allocated_block;
  MemoryHandle *candidate;
  int scan_mode;
  int result = MEM_MOVE_RESULT_MOVED;

  if(heap == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_MEMORY_BANK;
      return MEM_ERROR_NO_MEMORY_BANK;
    }

  bank_state = 0;
  if(block_kind == MEM_BLOCK_KIND_HANDLE)
    {
      free_block = heap->first_free_block;
      scan_mode = MEM_MOVE_TOWARD_LOW_ADDRESS;
    }
  else
    {
      free_block = heap->last_free_block;
      scan_mode = MEM_MOVE_TOWARD_HIGH_ADDRESS;
    }
  if(free_block == 0)
    {
      return 0;
    }

  candidate = 0;
  for(;;)
    {
      candidate = find_next_movable_memory_handle(
        heap, candidate, &bank_state, &bank_first_handle,
        &bank_last_handle, block_kind);
      if(candidate == 0)
        {
          break;
        }

      // Seed the first pass; only a successful move dirties the free list.
      if(result == MEM_MOVE_RESULT_MOVED)
        {
          coalesce_free_memory_blocks(heap);
        }

      if(block_kind == MEM_BLOCK_KIND_HANDLE)
        {
          free_block = heap->first_free_block;
        }
      else
        {
          free_block = heap->last_free_block;
        }
      if(free_block == 0)
        {
          break;
        }

      allocated_block = (MemoryBlockHeader *)
                        ((char *)candidate->data - MEM_POINTER_HEADER_SIZE);
      if(allocated_block->owner != candidate)
        {
          memory_system_last_error = MEM_ERROR_BAD_HANDLE;
          return MEM_ERROR_BAD_HANDLE;
        }

      for(;;)
        {
          result = MoveBlkToFreeSpace(
            heap, &allocated_block, &free_block, scan_mode);
          if(result == MEM_ERROR_BAD_HANDLE)
            {
              return MEM_ERROR_BAD_HANDLE;
            }
          if(result != MEM_MOVE_RESULT_RETRY)
            {
              break;
            }

          if(block_kind == MEM_BLOCK_KIND_HANDLE)
            {
              free_block = free_block->next;
            }
          else
            {
              free_block = free_block->previous;
            }
          if(free_block == 0)
            {
              break;
            }
        }
    }

  coalesce_free_memory_blocks(heap);
  rebuild_memory_heap_free_list(heap);
  return 0;
}
