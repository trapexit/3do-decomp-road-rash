#include "race_object_creation.h"

enum SortedIntrusiveListConstant
{
  SORTED_INTRUSIVE_LIST_BOOLEAN_MASK = 0xFF
};

IntrusiveListNode *
insert_intrusive_list_node_by_int_key(IntrusiveListHeader *list,
                                      IntrusiveListNode   *node,
                                      int                  key_offset,
                                      int                  sort_ascending)
{
  IntrusiveListNode *current;
  int ascending;
  int key;
  int current_key;

  ascending = sort_ascending & SORTED_INTRUSIVE_LIST_BOOLEAN_MASK;
  current = list->head_sentinel->next;
  key = *(int *)((unsigned char *)node + key_offset);

  while(current->next != 0)
    {
      current_key = *(int *)((unsigned char *)current + key_offset);
      if(ascending != 0)
        {
          if(current_key > key)
            {
              return insert_intrusive_list_node_before(current, node);
            }
        }
      else if(current_key < key)
        {
          return insert_intrusive_list_node_before(current, node);
        }
      current = current->next;
    }

  return append_intrusive_list_node(list, node);
}
