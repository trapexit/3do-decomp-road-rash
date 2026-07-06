#include "dll_runtime.h"

int
DLL_RemoveHead(DLLList *list,
               void   **out_value);

int
DLL_RemoveAll(DLLList *list)
{
  int count;
  int result;

  result = 0;
  if(list == 0)
    {
      return -1;
    }

  count = list->count;
  while(count != 0)
    {
      result = DLL_RemoveHead(list, 0);
      if(result != 0)
        {
          return result;
        }
      count--;
    }

  return result;
}
