#include "intrusive_list.h"
#include "memory_manager_runtime.h"

int
destroy_intrusive_object_pool(IntrusiveListStorage *storage)
{
  reset_intrusive_object_pool(storage);
  return MEM_DisposePointer(storage);
}
