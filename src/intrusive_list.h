#ifndef ROADRASH_INTRUSIVE_LIST_H
#define ROADRASH_INTRUSIVE_LIST_H

typedef struct IntrusiveListNode IntrusiveListNode;
typedef struct IntrusiveListHeader IntrusiveListHeader;
typedef struct IntrusiveListStorage IntrusiveListStorage;

struct IntrusiveListNode
{
  IntrusiveListNode *next;
  IntrusiveListNode *previous;
};

struct IntrusiveListHeader
{
  IntrusiveListNode *head_sentinel;
  IntrusiveListNode *tail_sentinel;
};

typedef struct IntrusiveListSentinel
{
  IntrusiveListNode links;
  int reserved;
} IntrusiveListSentinel;

struct IntrusiveListStorage
{
  IntrusiveListHeader header;
  IntrusiveListSentinel head_storage;
  IntrusiveListSentinel tail_storage;
  void *owned_allocation;
};

typedef struct PooledListNode
{
  IntrusiveListNode links;
  IntrusiveListHeader *owner_pool;
} PooledListNode;

typedef char IntrusiveListNodeSizeIs08[
    (sizeof(IntrusiveListNode) == 0x08) ? 1 : -1];

int
unlink_intrusive_list_node(IntrusiveListNode *node);
int
return_pooled_node_to_owner(PooledListNode *node);
IntrusiveListNode *
insert_intrusive_list_node_before(IntrusiveListNode *position,
                                  IntrusiveListNode *node);
void
insert_intrusive_list_node_after(IntrusiveListNode *position,
                                 IntrusiveListNode *node);
void
prepend_intrusive_list_node(IntrusiveListHeader *list,
                            IntrusiveListNode   *node);
IntrusiveListNode *
append_intrusive_list_node(IntrusiveListHeader *list,
                           IntrusiveListNode   *node);
IntrusiveListNode *
pop_front_intrusive_list_node(IntrusiveListHeader *list);
IntrusiveListNode *
pop_back_intrusive_list_node(IntrusiveListHeader *list);
void
move_all_intrusive_list_nodes(IntrusiveListHeader *source,
                              IntrusiveListHeader *destination);
IntrusiveListStorage *
initialize_intrusive_list_storage(IntrusiveListStorage *storage);
void
dispose_intrusive_list_storage_if_empty(IntrusiveListStorage *storage);
int
reset_intrusive_object_pool(IntrusiveListStorage *storage);
int
destroy_intrusive_object_pool(IntrusiveListStorage *storage);
void
return_all_intrusive_list_nodes_to_owner(IntrusiveListStorage *storage);

#endif
