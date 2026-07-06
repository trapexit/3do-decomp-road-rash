#include "dll_runtime.h"

int
DLL_IterReset(DLLIterator *iterator);
int
DLL_IterValid(DLLIterator *iterator);

int
DLL_IterFirst(DLLIterator *iterator)
{
  DLL_IterReset(iterator);
  if(DLL_IterValid(iterator) == 0)
    {
      return 0;
    }
  return (int)iterator->current->value;
}
