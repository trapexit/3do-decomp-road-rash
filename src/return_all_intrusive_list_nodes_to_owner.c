#include "intrusive_list.h"

void
return_all_intrusive_list_nodes_to_owner(IntrusiveListStorage *storage)
{
  PooledListNode *node;
  PooledListNode *next;

  node = (PooledListNode *)storage->header.head_sentinel->next;
  if(node->links.next == 0)
    {
      return;
    }

  do
    {
      next = (PooledListNode *)node->links.next;
      unlink_intrusive_list_node(&node->links);
      append_intrusive_list_node(node->owner_pool, &node->links);
      node = next;
    } while(node->links.next != 0);
}
