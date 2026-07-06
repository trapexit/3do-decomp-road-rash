#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"





static
MemoryBlockHeader *
find_free_block_in_memory_heaps(uint32 memory_type,
                                int    byte_count,
                                uint32 options,
                                int    block_kind)
{
  MemoryHeapDescriptor *heap;
  MemoryBlockHeader *result;

  result = 0;
  heap = (MemoryHeapDescriptor *)memory_allocator_globals.heap_list_head;
  while(heap != 0 && result == 0)
    {
      if((memory_type & heap->memory_type) != 0 || memory_type == 0)
        {
          result = FindFreeBlockFor(
            heap, byte_count, options, block_kind);
        }
      heap = heap->next;
    }
  return result;
}


MemoryBlockHeader *
FindFreeBlockForAlloc(int    byte_count,
                      uint32 memory_type,
                      uint32 options)
{
  MemoryHeapDescriptor *heap;
  MemoryBlockHeader *result;
  int block_kind;
  uint32 allow_failure;
  uint32 skip_purge;
  int reclaim_status;

  if(byte_count < MEM_MINIMUM_PAYLOAD_SIZE)
    {
      byte_count = MEM_MINIMUM_PAYLOAD_SIZE;
    }

  if((options & MEM_POINTER_OPTION_POINTER_BLOCK) == 0)
    {
      block_kind = MEM_BLOCK_KIND_HANDLE;
    }
  else
    {
      block_kind = MEM_BLOCK_KIND_POINTER;
    }

  if((options & MEM_ALLOCATION_OPTION_PURGE_FIRST) != 0)
    {
      if(memory_type == 0)
        {
          heap = (MemoryHeapDescriptor *)
                 memory_allocator_globals.heap_list_head;
          while(heap != 0)
            {
              PurgeAllBlksInBank(heap);
              heap = heap->next;
            }
        }
      else
        {
          heap = GetMaxMemBank(memory_type);
          if(heap != 0)
            {
              PurgeAllBlksInBank(heap);
            }
        }
    }

  result = find_free_block_in_memory_heaps(
    memory_type, byte_count, options, block_kind);
  if(result != 0)
    {
      return result;
    }

  heap = GetMaxMemBank(memory_type);
  if(heap == 0)
    {
      return 0;
    }

  allow_failure = options & MEM_POINTER_OPTION_ALLOW_FAILURE;
  skip_purge = options & MEM_ALLOCATION_OPTION_SKIP_PURGE;
  reclaim_status = MEM_RECLAIM_RESULT_COMPACTED;

  for(;;)
    {
      if(heap != 0 && result == 0 &&
         reclaim_status == MEM_RECLAIM_RESULT_COMPACTED)
        {
          result = FindFreeBlockFor(
            heap, byte_count, options, block_kind);
          if(result != 0)
            {
              return result;
            }
        }

      if(heap == 0)
        {
          break;
        }
      if(result != 0)
        {
          return result;
        }

      if(allow_failure != 0 || skip_purge != 0)
        {
          return 0;
        }

      reclaim_status = PurgeSomeBlocks(
        memory_type, &heap, byte_count, block_kind, options);
      if(reclaim_status == MEM_ERROR_INVALID_HANDLE)
        {
          return 0;
        }
      if(reclaim_status != MEM_RECLAIM_RESULT_COMPACTED)
        {
          break;
        }
    }

  return find_free_block_in_memory_heaps(
    memory_type, byte_count, options, block_kind);
}
