#include "rw_semantic_data.h"

int
DLL_GetErr(void)
{
  int result;

  result = linked_list_memory_api.last_error;
  linked_list_memory_api.last_error = 0;
  return result;
}
