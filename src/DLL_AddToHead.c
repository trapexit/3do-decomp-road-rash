#include "dll_runtime.h"

DLLNode *
DLL_NewNode(int value);
int
DLL_GetErr(void);
int
DLL_IncrementCount(DLLList *list);

int
DLL_AddToHead(DLLList *list,
              int      value)
{
  DLLNode *node;
  int result;

  node = 0;
  result = 0;
  if(list == 0)
    {
      result = DLL_ERROR_NULL_ARGUMENT;
    }
  else
    {
      node = DLL_NewNode(value);
      if(node == 0)
        {
          result = DLL_GetErr();
        }
      else
        {
          node->next = list->head;
          node->previous = 0;
          if(list->head != 0)
            {
              list->head->previous = node;
            }
          list->head = node;
          DLL_IncrementCount(list);
          if(list->count == 1)
            {
              list->tail = list->head;
            }
        }
    }

  return result;
}
