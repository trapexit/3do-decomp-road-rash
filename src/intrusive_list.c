/* Intrusive-list primitives reconstructed from the 0x12CA4 code cluster. */

#include "stddef.h"

#include "intrusive_list.h"

typedef char IntrusiveListPreviousOffsetIs04[
    (offsetof(IntrusiveListNode, previous) == 0x04) ? 1 : -1];
typedef char PooledListNodeOwnerOffsetIs08[
    (offsetof(PooledListNode, owner_pool) == 0x08) ? 1 : -1];
typedef char IntrusiveListHeadStorageOffsetIs08[
    (offsetof(IntrusiveListStorage, head_storage) == 0x08) ? 1 : -1];
typedef char IntrusiveListTailStorageOffsetIs14[
    (offsetof(IntrusiveListStorage, tail_storage) == 0x14) ? 1 : -1];
typedef char IntrusiveListOwnedAllocationOffsetIs20[
    (offsetof(IntrusiveListStorage, owned_allocation) == 0x20) ? 1 : -1];
typedef char IntrusiveListStorageSizeIs24[
    (sizeof(IntrusiveListStorage) == 0x24) ? 1 : -1];

int
unlink_intrusive_list_node(IntrusiveListNode *node)
{
  IntrusiveListNode **next_links;
  IntrusiveListNode **previous_links;

  next_links = (IntrusiveListNode **)node->next;
  previous_links = (IntrusiveListNode **)node->previous;
  previous_links[0] = (IntrusiveListNode *)next_links;
  next_links[1] = (IntrusiveListNode *)previous_links;
  return (int)(next_links + 1);
}


int
return_pooled_node_to_owner(PooledListNode *node)
{
  unlink_intrusive_list_node(&node->links);
  return (int)append_intrusive_list_node(node->owner_pool, &node->links);
}
