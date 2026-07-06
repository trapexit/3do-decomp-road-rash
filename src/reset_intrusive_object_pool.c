#include "intrusive_list.h"
#include "memory_manager_runtime.h"

int
reset_intrusive_object_pool(IntrusiveListStorage *storage)
{
  MEM_DisposePointer(storage->owned_allocation);
  initialize_intrusive_list_storage(storage);
  storage->owned_allocation = 0;
  return 0;
}
