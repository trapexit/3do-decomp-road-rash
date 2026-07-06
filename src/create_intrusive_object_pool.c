#include "intrusive_list.h"
#include "memory_manager_runtime.h"

char *
create_intrusive_object_pool(int   element_size,
                             int   count,
                             char *initial_node)
{
  char *block;
  IntrusiveListStorage *storage;
  PooledListNode *current;
  int i;

  if(element_size < (int)sizeof(PooledListNode) || count < 0 ||
     (element_size & MEM_ALLOCATION_ALIGNMENT_MASK) != 0 ||
     (uint32)count >
     (MEM_BLOCK_SIZE_MASK - MEM_POINTER_HEADER_SIZE -
      MEM_ALLOCATION_ALIGNMENT_MASK) / (uint32)element_size)
    {
      return 0;
    }

  block = MEM_NewPointer(element_size * count, MEMORY_REQUEST_ANY,
                         MEM_POINTER_OPTIONS_DEFAULT);
  if(block == 0)
    {
      return 0;
    }

  storage = initialize_intrusive_list_storage(
    (IntrusiveListStorage *)initial_node);
  if(storage == 0)
    {
      MEM_DisposePointer(block);
      return 0;
    }

  storage->owned_allocation = block;

  current = (PooledListNode *)block;
  for(i = 0; i < count; i++)
    {
      append_intrusive_list_node(&storage->header, &current->links);
      current->owner_pool = &storage->header;
      current = (PooledListNode *)((char *)current + element_size);
    }

  return (char *)storage;
}
