#include "dll_runtime.h"
#include "rw_semantic_data.h"
#include "stdio.h"


int
DLL_DecrementCount(DLLList *list)
{
  int count;
  FILE *stream;

  stream = stdout;
  if(list == 0)
    {
      printf("List NULL in DecItemCount()\n");
      return fflush(stream);
    }

  count = list->count - 1;
  list->count = count;
  if(count >= 0)
    {
      return count;
    }

  printf("Underflow in DecItemCount()\n");
  fflush(stream);
  list->count = 0;
  return 0;
}
