#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
MEM_PrepHandleLock(MemoryHandle *handle)
{
  MemoryHeapDescriptor *heap;
  int result;
  MemoryBlockHeader *block;
  MemoryBlockHeader *free_block;

  if(handle == 0 || handle->data == 0 ||
     (handle->flags & MEM_HANDLE_FLAG_VALID) == 0)
    {
      return MEM_ERROR_BAD_HANDLE;
    }

  handle->flags &= ~MEM_HANDLE_FLAG_BUSY;
  block = (MemoryBlockHeader *)((char *)handle->data -
                                MEM_POINTER_HEADER_SIZE);
  heap = find_memory_bank_containing_address((uint32)block);
  result = CompactBank(heap, MEM_BLOCK_KIND_HANDLE);
  if(result != 0)
    {
      return memory_system_last_error;
    }

  block = (MemoryBlockHeader *)((char *)handle->data -
                                MEM_POINTER_HEADER_SIZE);
  free_block = heap->last_free_block;
  for(;;)
    {
      result = MoveBlkToFreeSpace(
        heap, &block, &free_block,
        MEM_MOVE_TOWARD_HIGH_ADDRESS);
      if(result == MEM_ERROR_BAD_HANDLE)
        {
          return MEM_ERROR_BAD_HANDLE;
        }

      free_block = free_block->previous;
      if(result != MEM_MOVE_RESULT_RETRY)
        {
          return 0;
        }
      if(free_block == 0)
        {
          return MEM_ERROR_NO_FREE_BLOCK;
        }
    }
}
