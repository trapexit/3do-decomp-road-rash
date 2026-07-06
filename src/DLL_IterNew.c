#include "dll_runtime.h"
#include "rw_semantic_data.h"

int
DLL_SetErr(int error_);
int
DLL_IterReset(DLLIterator *iterator);

DLLIterator *
DLL_IterNew(DLLList *list,
            int      forward)
{
  DLLIterator *iterator;
  int (*alloc_fn)(int,
                  int);

  if(list == 0)
    {
      DLL_SetErr(DLL_ERROR_NO_MEMORY);
      return 0;
    }

  alloc_fn = linked_list_memory_api.allocate;
  iterator = (DLLIterator *)alloc_fn(sizeof(DLLIterator), 0);
  if(iterator == 0)
    {
      DLL_SetErr(DLL_ERROR_NO_MEMORY);
      return 0;
    }

  iterator->list = list;
  iterator->forward = (unsigned char)forward;
  DLL_IterReset(iterator);
  return iterator;
}
