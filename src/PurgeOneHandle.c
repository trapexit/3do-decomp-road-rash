#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

static
uint32
purge_handle_memory_type_flag(const MemoryHandle *handle)
{
  const MemoryBlockHeader *block;

  block = (const MemoryBlockHeader *)(
    (const char *)handle->data - MEM_POINTER_HEADER_SIZE);
  if((block->size_and_flags & MEM_BLOCK_VRAM_FLAG) == 0)
    {
      return MEM_BLOCK_ALLOCATE_HIGH_FLAG;
    }
  return MEM_BLOCK_ALLOCATE_LOW_FLAG;
}


MemoryHandle *
PurgeOneHandle(uint32                 requested_memory_flags,
               MemoryHeapDescriptor **heap_ref,
               MemoryHandle          *handle,
               MemoryHandle         **previous_ref,
               int                   *purged_count,
               uint32                 allocation_options)
{
  MemoryHandle *next;
  MemoryHeapDescriptor *heap;
  uint32 memory_type_flag;
  uint32 block_address;
  uint32 heap_start;
  uint32 heap_end;

  memory_type_flag = purge_handle_memory_type_flag(handle);
  while(handle != 0)
    {
      if((memory_type_flag & requested_memory_flags) == 0 ||
         requested_memory_flags == 0)
        {
          break;
        }
      if((allocation_options &
          MEM_ALLOCATION_OPTION_PRESERVE_SECONDARY_PURGE) == 0 ||
         (handle->flags & MEM_HANDLE_FLAG_SECONDARY_MEMORY) == 0)
        {
          break;
        }

      *previous_ref = handle;
      handle = GetNextPurgeNode(handle);
      if(handle == 0)
        {
          return 0;
        }
      memory_type_flag = purge_handle_memory_type_flag(handle);
    }

  if(handle == 0)
    {
      return 0;
    }

  next = GetNextPurgeNode(handle);
  SkipPurgeNode(*previous_ref, handle);

  block_address =
    (uint32)handle->data - MEM_POINTER_HEADER_SIZE;
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  while(heap != 0)
    {
      heap_start = (uint32)heap->base_address;
      if(heap_start <= block_address)
        {
          heap_end = heap_start + heap->size;
          if(block_address <= heap_end)
            {
              break;
            }
        }
      heap = heap->next;
    }

  *heap_ref = heap;
  AddHandleToFreeSpace(handle, heap);
  *purged_count += 1;
  return next;
}
