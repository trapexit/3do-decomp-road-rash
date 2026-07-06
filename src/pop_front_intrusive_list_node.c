#include "intrusive_list.h"

IntrusiveListNode *
pop_front_intrusive_list_node(IntrusiveListHeader *list)
{
  IntrusiveListNode *node;

  node = list->head_sentinel->next;
  if(node == list->tail_sentinel)
    {
      return 0;
    }

  unlink_intrusive_list_node(node);
  return node;
}
