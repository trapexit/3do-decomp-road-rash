#include "dll_runtime.h"
#include "rw_semantic_data.h"

int
DLL_SetCompare(DLLList           *list,
               DLLCompareFunction compare)
{
  if(list != 0)
    {
      list->compare = compare;
      return (int)&list->compare;
    }
  linked_list_memory_api.last_error = DLL_ERROR_NULL_ARGUMENT;
  return (int)&linked_list_memory_api.last_error;
}
