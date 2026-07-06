#include "dll_runtime.h"
#include "rw_semantic_data.h"

void
DLL_IterDelete(DLLIterator *iterator)
{
  if(iterator == 0)
    {
      return;
    }
  linked_list_memory_api.release((int)iterator, sizeof(DLLIterator));
}
