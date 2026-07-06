#include "dll_runtime.h"
#include "rw_semantic_data.h"

int
DLL_SetErr(int error);

DLLNode *
DLL_NewNode(int value)
{
  DLLNode *node;
  int (*alloc_func)(int,
                    int);

  linked_list_memory_api.last_error = 0;
  alloc_func = linked_list_memory_api.allocate;
  node = (DLLNode *)alloc_func(sizeof(DLLNode), 0);
  if(node == 0)
    {
      DLL_SetErr(DLL_ERROR_NO_MEMORY);
    }
  else
    {
      node->previous = 0;
      node->value = (void *)value;
      node->next = 0;
    }

  return node;
}
