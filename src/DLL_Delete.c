#include "dll_runtime.h"
#include "rw_semantic_data.h"

int
DLL_RemoveAll(DLLList *list);

int
DLL_Delete(DLLList *list)
{
  int result;

  result = 0;
  if(list == 0)
    {
      return -1;
    }

  if(list->count != 0)
    {
      result = DLL_RemoveAll(list);
      if(result != 0)
        {
          return result;
        }
    }

  linked_list_memory_api.release((int)list, sizeof(DLLList));
  return result;
}
