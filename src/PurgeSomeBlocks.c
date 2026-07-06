/* PurgeSomeBlocks - original function at 0x000354B8. */



#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

int
PurgeSomeBlocks(uint32                 memory_type,
                MemoryHeapDescriptor **heap_ref,
                int                    byte_count,
                int                    block_kind,
                uint32                 options)
{
  MemoryHandle *handle;
  MemoryHandle *previous;
  int purged_count;
  uint32 required_byte_count;

  handle = (MemoryHandle *)memory_allocator_globals.block_list_head;
  if(handle == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return MEM_RECLAIM_RESULT_UNAVAILABLE;
    }

  previous = 0;
  *heap_ref = GetMaxMemBank(memory_type);
  if(*heap_ref == 0)
    {
      return MEM_RECLAIM_RESULT_UNAVAILABLE;
    }

  purged_count = 0;
  required_byte_count =
    (uint32)byte_count + MEM_POINTER_HEADER_SIZE;
  do
    {
      handle = PurgeOneHandle(
        memory_type, heap_ref, handle, &previous,
        &purged_count, options);
      if(handle == 0)
        {
          break;
        }
    } while(required_byte_count > (*heap_ref)->free_bytes);

  if(purged_count <= 0)
    {
      return MEM_RECLAIM_RESULT_UNAVAILABLE;
    }
  if(CompactBank(*heap_ref, block_kind) != 0)
    {
      return MEM_ERROR_INVALID_HANDLE;
    }
  return MEM_RECLAIM_RESULT_COMPACTED;
}
