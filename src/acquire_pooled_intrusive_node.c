#include "intrusive_list.h"

IntrusiveListNode *
acquire_pooled_intrusive_node(IntrusiveListHeader *object_pool)
{
  return pop_back_intrusive_list_node(object_pool);
}
