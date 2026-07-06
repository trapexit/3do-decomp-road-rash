#include "dll_runtime.h"

DLLNode **
DLL_IterAdvance(DLLIterator *iterator);
int
DLL_IterValid(DLLIterator *iterator);

int
DLL_IterNext(DLLIterator *iterator)
{
  DLL_IterAdvance(iterator);
  if(DLL_IterValid(iterator) == 0)
    {
      return 0;
    }
  return (int)iterator->current->value;
}
