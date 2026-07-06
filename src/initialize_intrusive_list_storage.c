#include "intrusive_list.h"
#include "memory_manager_runtime.h"

enum IntrusiveListStorageConstant
{
  INTRUSIVE_LIST_STORAGE_SIZE = 0x24
};

IntrusiveListStorage *
initialize_intrusive_list_storage(IntrusiveListStorage *storage)
{
  IntrusiveListNode *head;
  IntrusiveListNode *tail;

  if(storage == 0)
    {
      storage = (IntrusiveListStorage *)MEM_NewPointer(
        INTRUSIVE_LIST_STORAGE_SIZE, MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      if(storage == 0)
        {
          return 0;
        }
      storage->owned_allocation = 0;
    }

  head = &storage->head_storage.links;
  tail = &storage->tail_storage.links;

  storage->header.head_sentinel = head;
  storage->header.tail_sentinel = tail;
  head->next = tail;
  tail->previous = head;
  head->previous = 0;
  tail->next = 0;

  return storage;
}
