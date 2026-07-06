#include "dll_runtime.h"

int
DLL_IterReset(DLLIterator *iterator)
{
  DLLList *list;

  if(iterator == 0)
    {
      return (int)iterator;
    }
  list = iterator->list;
  if(list == 0)
    {
      return (int)iterator;
    }
  if(iterator->forward == 0)
    {
      iterator->current = list->tail;
    }
  else
    {
      iterator->current = list->head;
    }
  return (int)&iterator->current;
}
