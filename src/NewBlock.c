#include "item.h"
/* NewBlock - reconstructed from the 0x00036434 routine. */

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"

MemoryBlockHeader *
FindFreeBlockForAlloc(int    byte_count,
                      uint32 placement_flags,
                      uint32 options);
MemoryBlockHeader *
GetPtrFromBlock(MemoryBlockHeader *free_block,
                int                byte_count,
                uint32             placement_flags,
                int                allocation_kind);




MemoryBlockHeader *
NewBlock(int    byte_count,
         uint32 placement_flags,
         uint32 options)
{
  MemoryBlockHeader *free_block;
  int allocation_kind;
  MemoryBlockHeader *allocation;

  if(byte_count < 0 ||
     (uint32)byte_count >
     MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
     MEM_ALLOCATION_ALIGNMENT_MASK)
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return 0;
    }

  free_block = FindFreeBlockForAlloc(
    byte_count, placement_flags, options);
  if(free_block == 0)
    {
      memory_system_last_error = MEM_ERROR_NO_FREE_BLOCK;
      return 0;
    }

  if((options & MEM_POINTER_OPTION_POINTER_BLOCK) == 0)
    {
      allocation_kind = MEM_BLOCK_KIND_HANDLE;
    }
  else
    {
      allocation_kind = MEM_BLOCK_KIND_POINTER;
    }

  allocation = GetPtrFromBlock(
    free_block, byte_count, placement_flags, allocation_kind);
  if(allocation != 0)
    {
      return allocation;
    }

  return 0;
}
