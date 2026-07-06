#include "dll_runtime.h"

int
DLL_FindIndexedNode(DLLList  *list,
                    int       index,
                    DLLNode **out_node);

int
DLL_GetIndexed(DLLList *list,
               int      index,
               void   **out_value)
{
  DLLNode *node;
  int result;

  node = 0;
  result = DLL_FindIndexedNode(list, index, &node);
  if(result == 0 && node != 0)
    {
      *out_value = node->value;
    }
  return result;
}
