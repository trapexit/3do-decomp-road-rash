#include "intrusive_list.h"
#include "memory_manager_runtime.h"

void
dispose_intrusive_list_storage_if_empty(IntrusiveListStorage *storage)
{
  IntrusiveListNode *head;

  head = storage->header.head_sentinel;
  if(head->next == storage->header.tail_sentinel)
    {
      MEM_DisposePointer(storage);
    }
}
