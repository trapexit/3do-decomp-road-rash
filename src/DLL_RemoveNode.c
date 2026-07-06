#include "dll_runtime.h"

int
DLL_IsEmpty(DLLList *list);
int
DLL_DecrementCount(DLLList *list);
void
DLL_DeleteNode(DLLNode *node);

int
DLL_RemoveNode(DLLList *list,
               DLLNode *node,
               void   **out_value)
{
  int result;
  DLLNode *next;
  DLLNode *previous;

  result = 0;
  if(list == 0 || node == 0)
    {
      return DLL_ERROR_NULL_ARGUMENT;
    }

  if(DLL_IsEmpty(list) != 0)
    {
      return DLL_ERROR_EMPTY_LIST;
    }

  if(node == list->head)
    {
      next = node->next;
      list->head = next;
      if(next != 0)
        {
          next->previous = 0;
        }
      node->previous = 0;
      node->next = 0;
      DLL_DecrementCount(list);
      if(DLL_IsEmpty(list) != 0)
        {
          list->tail = 0;
        }
    }
  else if(node == list->tail)
    {
      previous = node->previous;
      list->tail = previous;
      if(previous != 0)
        {
          previous->next = 0;
        }
      node->previous = 0;
      node->next = 0;
      DLL_DecrementCount(list);
      if(DLL_IsEmpty(list) != 0)
        {
          list->head = 0;
        }
    }
  else
    {
      previous = node->previous;
      next = node->next;
      previous->next = next;
      next->previous = previous;
      node->previous = 0;
      node->next = 0;
      DLL_DecrementCount(list);
    }

  if(out_value != 0)
    {
      *out_value = node->value;
    }
  DLL_DeleteNode(node);
  return result;
}
