#include "dll_runtime.h"

int
DLL_IterValid(DLLIterator *iterator)
{
  int result;

  result = 0;
  if(iterator != 0 && iterator->current != 0)
    {
      result = 1;
    }
  return (unsigned char)result;
}
