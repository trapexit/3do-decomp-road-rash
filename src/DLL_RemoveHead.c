#include "dll_runtime.h"
#include "rw_semantic_data.h"
#include "stdio.h"

int
DLL_RemoveNode(DLLList *list,
               DLLNode *node,
               void   **out_value);
int
DLL_FindIndexedNode(DLLList  *list,
                    int       index,
                    DLLNode **out_node);

int
DLL_RemoveHead(DLLList *list,
               void   **out_value)
{
  DLLNode *node;

  if(list == 0)
    {
      node = 0;
    }
  else
    {
      node = list->head;
    }

  return DLL_RemoveNode(list, node, out_value);
}


int
DLL_RemoveTail(DLLList *list,
               void   **out_value)
{
  DLLNode *node;

  if(list == 0)
    {
      node = 0;
    }
  else
    {
      node = list->tail;
    }

  return DLL_RemoveNode(list, node, out_value);
}


int
DLL_RemoveIndexed(DLLList *list,
                  void   **out_value,
                  int      index)
{
  DLLNode *node;
  int result;

  node = 0;
  if(out_value == 0)
    {
      return -1;
    }

  result = DLL_FindIndexedNode(list, index, &node);
  if(result != 0)
    {
      return result;
    }

  if(node == 0)
    {
      printf("node Ptr NULL in DLL_RemoveIndexed");
      fflush(stdout);
      return 0;
    }

  return DLL_RemoveNode(list, node, out_value);
}
