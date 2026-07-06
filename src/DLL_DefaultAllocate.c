#include "dll_runtime.h"
#include "rw_semantic_data.h"
#include "stdlib.h"

static
void
release_memory(void *memory)
{
  free(memory);
}


void
DLL_DeleteNode(DLLNode *node)
{
  if(node == 0)
    {
      return;
    }
  linked_list_memory_api.release((int)node, sizeof(DLLNode));
}


int
DLL_DefaultAllocate(int size,
                    int memory_type)
{
  (void)memory_type;
  return (int)malloc(size);
}


void
DLL_DefaultRelease(int memory,
                   int size)
{
  (void)size;
  release_memory((void *)memory);
}
