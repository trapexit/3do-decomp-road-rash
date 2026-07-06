#include "intrusive_list.h"

void
move_all_intrusive_list_nodes(IntrusiveListHeader *source,
                              IntrusiveListHeader *destination)
{
  IntrusiveListNode *node;
  IntrusiveListNode *next;

  node = source->head_sentinel->next;
  if(node->next == 0)
    {
      return;
    }

  do
    {
      next = node->next;
      unlink_intrusive_list_node(node);
      append_intrusive_list_node(destination, node);
      node = next;
    } while(node->next != 0);
}
