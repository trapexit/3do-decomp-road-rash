#include "intrusive_list.h"

IntrusiveListNode *
append_intrusive_list_node(IntrusiveListHeader *list,
                           IntrusiveListNode   *node)
{
  return insert_intrusive_list_node_before(list->tail_sentinel, node);
}
