#include "dll_runtime.h"

int
DLL_FindIndexedNode(DLLList  *list,
                    int       requested_index,
                    DLLNode **out_node)
{
  DLLNode *node;
  int index;

  if(list == 0 || out_node == 0)
    {
      return DLL_ERROR_NULL_ARGUMENT;
    }
  if(requested_index < 0 || list->count <= requested_index)
    {
      return DLL_ERROR_BAD_INDEX;
    }

  node = list->head;
  index = 0;
  while(node != 0 && index < requested_index)
    {
      node = node->next;
      index += 1;
    }

  *out_node = node;
  return 0;
}
