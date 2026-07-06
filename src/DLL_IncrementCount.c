#include "dll_runtime.h"
#include "rw_semantic_data.h"
#include "stdio.h"


int
DLL_IncrementCount(DLLList *list)
{
  if(list != 0)
    {
      list->count++;
      return (int)list;
    }

  printf("List NULL in IncItemCount()\n");
  return fflush(stdout);
}
