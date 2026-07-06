#include "memory_manager_runtime.h"

void *
AllocPoolMem(MemPoolPtr pool)
{
  MemPoolEntryPtr entry;

  entry = pool->availList;
  if(entry != 0)
    {
      pool->availList = entry->next;
      pool->numFreeInPool--;
      return entry->data;
    }

  return 0;
}


void
ReturnPoolMem(MemPoolPtr pool,
              void      *pool_entry)
{
  MemPoolEntryPtr entry;

  entry = (MemPoolEntryPtr)((char *)pool_entry -
                            sizeof(MemPoolEntryPtr));
  entry->next = pool->availList;
  pool->availList = entry;
  pool->numFreeInPool++;
}


boolean
ForEachFreePoolMember(MemPoolPtr                   pool,
                      ForEachFreePoolMemberFuncPtr for_each,
                      void                        *argument)
{
  MemPoolEntryPtr entry;

  entry = pool->availList;
  while(entry != 0)
    {
      if(for_each(argument, entry->data) == false)
        {
          return false;
        }
      entry = entry->next;
    }

  return true;
}
