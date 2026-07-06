#include "dll_runtime.h"

DLLNode **
DLL_IterAdvance(DLLIterator *iterator)
{
  DLLNode *next;
  DLLNode *current;

  next = 0;
  current = iterator->current;
  if(iterator->forward != 0)
    {
      if(current != 0)
        {
          next = current->next;
        }
    }
  else if(current != 0)
    {
      next = current->previous;
    }

  iterator->current = next;
  return &iterator->current;
}
