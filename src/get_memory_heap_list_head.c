#include "rw_semantic_data.h"

int
get_memory_heap_list_head(void)
{
  return (int)memory_allocator_globals.heap_list_head;
}
