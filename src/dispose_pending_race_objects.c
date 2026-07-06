#include "intrusive_list.h"
#include "race_object_lifecycle.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum RaceObjectDisposalValue
{
  RACE_OBJECT_RETAINED_FLAG = 0x04,
  RACE_OBJECT_POOL_ALLOCATION_FLAG = 0x08
};


void
dispose_pending_race_objects(void)
{
  IntrusiveListNode *node;
  IntrusiveListNode *next;
  RacerEntity *object;

  race_object_scheduler.removal_pending = 0;

  node = race_object_scheduler.object_list->head_sentinel->next;
  if(node->next == 0)
    {
      return;
    }

  do
    {
      next = node->next;
      object = (RacerEntity *)node;
      if((object->status_flags & RACE_OBJECT_RETAINED_FLAG) == 0)
        {
          if(node == race_object_scheduler.first_visible_object)
            {
              race_object_scheduler.first_visible_object = node->previous;
            }
          if(node == race_object_scheduler.last_visible_object)
            {
              race_object_scheduler.last_visible_object = node->previous;
            }

          if((object->status_flags &
              RACE_OBJECT_POOL_ALLOCATION_FLAG) != 0)
            {
              return_pooled_node_to_owner(&object->list_node);
            }
          else
            {
              unlink_intrusive_list_node(node);
            }
        }
      node = next;
    } while(node->next != 0);
}

