#include "intrusive_list.h"

IntrusiveListNode *
insert_intrusive_list_node_before(IntrusiveListNode *position,
                                  IntrusiveListNode *node)
{
  IntrusiveListNode *previous;

  previous = position->previous;
  previous->next = node;
  node->previous = previous;
  position->previous = node;
  node->next = position;
  return position;
}
