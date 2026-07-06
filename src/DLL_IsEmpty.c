#include "dll_runtime.h"

int
DLL_SetErr(int error_);

int
DLL_IsEmpty(DLLList *list)
{
  if(list == 0)
    {
      DLL_SetErr(DLL_ERROR_NULL_ARGUMENT);
      return 1;
    }
  return list->count <= 0;
}
