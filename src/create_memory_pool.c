#include "memory_manager_runtime.h"

MemPoolPtr
CreateMemPool(long count,
              long entry_size)
{
  long stride;
  MemPoolPtr pool;
  MemPoolEntryPtr entry;
  MemPoolEntryPtr previous_entry;
  long remaining;

  if(count < 0 || entry_size < 0 ||
     (uint32)entry_size >
     MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
     MEM_ALLOCATION_ALIGNMENT_MASK - sizeof(MemPoolEntryPtr))
    {
      return 0;
    }

  stride = ((entry_size + MEM_ALLOCATION_ALIGNMENT_MASK) &
            ~(long)MEM_ALLOCATION_ALIGNMENT_MASK) +
           sizeof(MemPoolEntryPtr);
  if((uint32)count >
     (MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
      MEM_ALLOCATION_ALIGNMENT_MASK - sizeof(MemPool)) /
     (uint32)stride)
    {
      return 0;
    }
  pool = (MemPoolPtr)MEM_NewPointer(
    stride * count + sizeof(MemPool), MEMTYPE_ANY,
    MEM_POINTER_OPTIONS_DEFAULT);
  if(pool == 0)
    {
      return 0;
    }

  pool->numItemsInPool = count;
  pool->numFreeInPool = count;
  entry = (MemPoolEntryPtr)pool->data;
  previous_entry = 0;
  remaining = count;
  while(remaining > 0)
    {
      entry->next = previous_entry;
      previous_entry = entry;
      entry = (MemPoolEntryPtr)((char *)entry + stride);
      remaining--;
    }
  pool->availList = previous_entry;

  return pool;
}
