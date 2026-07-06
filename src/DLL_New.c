#include "dll_runtime.h"
#include "rw_semantic_data.h"


int
DLL_DefaultAllocate(int size,
                    int memory_type);
void
DLL_DefaultRelease(int memory,
                   int size);

DLLList *
DLL_New(RwMemoryAllocator allocator,
        RwMemoryReleaser  releaser)
{
  DLLList *list;
  RwMemoryAllocator alloc_fn;

  linked_list_memory_api.last_error = 0;

  if((allocator == 0 && releaser != 0) ||
     (allocator != 0 && releaser == 0))
    {
      linked_list_memory_api.last_error = -1;
      return 0;
    }

  if(allocator == 0)
    {
      alloc_fn = DLL_DefaultAllocate;
    }
  else
    {
      alloc_fn = allocator;
    }
  linked_list_memory_api.allocate = alloc_fn;

  if(releaser == 0)
    {
      linked_list_memory_api.release = DLL_DefaultRelease;
    }
  else
    {
      linked_list_memory_api.release = releaser;
    }

  list = (DLLList *)alloc_fn(sizeof(DLLList), 0);
  if(list == 0)
    {
      linked_list_memory_api.last_error = DLL_ERROR_NO_MEMORY;
      return 0;
    }

  list->count = 0;
  list->head = 0;
  list->tail = 0;
  list->compare = 0;
  return list;
}
