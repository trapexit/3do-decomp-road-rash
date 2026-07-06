#include "dll_runtime.h"

int
DLL_AddTail(DLLList *list,
            int      value);
int
DLL_AddToHead(DLLList *list,
              int      value);
DLLNode *
DLL_NewNode(int value);
int
DLL_GetErr(void);
int
DLL_IncrementCount(DLLList *list);

int
DLL_Add(DLLList *list,
        int      value)
{
  DLLCompareFunction compare;
  DLLNode *node;
  DLLNode *insert;
  int result;

  result = 0;
  if(list == 0)
    {
      return DLL_ERROR_NULL_ARGUMENT;
    }

  compare = list->compare;
  node = list->head;
  if(compare == 0 || node == 0)
    {
      return DLL_AddTail(list, value);
    }

  while(node != 0)
    {
      if(compare(value, (int)node->value) <= 0)
        {
          break;
        }
      node = node->next;
    }

  if(node == list->head)
    {
      result = DLL_AddToHead(list, value);
    }
  else if(node == 0)
    {
      result = DLL_AddTail(list, value);
    }
  else
    {
      insert = DLL_NewNode(value);
      if(insert == 0)
        {
          result = DLL_GetErr();
        }
      else
        {
          insert->next = node;
          insert->previous = node->previous;
          node->previous = insert;
          insert->previous->next = insert;
          DLL_IncrementCount(list);
        }
    }

  return result;
}
