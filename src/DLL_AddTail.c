#include "dll_runtime.h"

DLLNode *
DLL_NewNode(int value);
int
DLL_GetErr(void);
int
DLL_IncrementCount(DLLList *list);

int
DLL_AddTail(DLLList *list,
            int      value)
{
  DLLNode *node;
  int result;

  result = 0;
  if(list == 0)
    {
      return -1;
    }

  node = DLL_NewNode(value);
  if(node == 0)
    {
      result = DLL_GetErr();
    }
  else
    {
      node->next = 0;
      node->previous = list->tail;
      if(list->tail != 0)
        {
          list->tail->next = node;
        }

      list->tail = node;
      DLL_IncrementCount(list);
      if(list->count == 1)
        {
          list->head = list->tail;
        }
    }

  return result;
}
