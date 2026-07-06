#include "dll_runtime.h"

int
DLL_SetErr(int error_);

int
DLL_GetItemCount(DLLList *list)
{
  if(list != 0)
    {
      return list->count;
    }
  DLL_SetErr(DLL_ERROR_NULL_ARGUMENT);
  return DLL_ERROR_NULL_ARGUMENT;
}
