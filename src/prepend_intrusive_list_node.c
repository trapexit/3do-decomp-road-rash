#include "intrusive_list.h"

void
prepend_intrusive_list_node(IntrusiveListHeader *list,
                            IntrusiveListNode   *node)
{
  insert_intrusive_list_node_after(list->head_sentinel, node);
}
