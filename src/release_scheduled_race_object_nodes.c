#include "intrusive_list.h"
#include "rw_semantic_data.h"

int
release_scheduled_race_object_nodes(void)
{
  int *node;
  int *next;

  if(race_object_scheduler.object_list == 0)
    {
      return 0;
    }
  node = **(int ***)race_object_scheduler.object_list;
  if(*node == 0)
    {
      return 0;
    }

  do
    {
      next = (int *)*node;
      return_pooled_node_to_owner((PooledListNode *)node);
      node = next;
    } while(*node != 0);

  return 0;
}
