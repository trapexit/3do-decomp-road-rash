#include "item.h"

#include "rw_semantic_data.h"

void
synchronize_memory_allocator(void)
{
  LockItem(memory_allocator_globals.lock_item, 1);
  UnlockItem(memory_allocator_globals.lock_item);
}


