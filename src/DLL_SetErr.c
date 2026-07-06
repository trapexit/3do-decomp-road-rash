#include "rw_semantic_data.h"


int
DLL_SetErr(int error_)
{
  linked_list_memory_api.last_error = error_;
  return 0;
}
