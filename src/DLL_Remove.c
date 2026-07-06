#include "dll_runtime.h"

int
DLL_RemoveNode(DLLList *list,
               DLLNode *node,
               void   **out_value);

int
DLL_Remove(DLLList *list,
           int      value)
{
  int result;
  DLLNode *node;

  result = 0;
  if(list == 0)
    {
      return DLL_ERROR_NULL_ARGUMENT;
    }

  node = list->head;
  while(node != 0)
    {
      if((int)node->value == value)
        {
          result = DLL_RemoveNode(list, node, 0);
          break;
        }
      node = node->next;
    }

  return result;
}
