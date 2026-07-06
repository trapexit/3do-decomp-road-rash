#include "item.h"

#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"



MemoryHandle *
MEM_PointerToHandle(void *pointer)
{
  MemoryBlockHeader *block;
  MemoryHandle *result;

  if(pointer == 0)
    {
      return 0;
    }

  LockItem(memory_allocator_globals.lock_item, 1);
  block = (MemoryBlockHeader *)((char *)pointer -
                                MEM_POINTER_HEADER_SIZE);
  result = GetNewMasterPtr(block);
  if(result == 0)
    {
      UnlockItem(memory_allocator_globals.lock_item);
      return 0;
    }

  block->owner = result;
  block->size_and_flags |= MEM_BLOCK_HANDLE_OWNER_FLAG;
  UnlockItem(memory_allocator_globals.lock_item);
  return result;
}

