#include "stddef.h"

#include "race_object_creation.h"
#include "rw_semantic_data.h"

enum ScheduledRaceObjectFlag
{
  SCHEDULED_RACE_OBJECT_RETAINED = 0x04,
  SCHEDULED_RACE_OBJECT_PENDING_POOL_RETURN = 0x08
};

IntrusiveListNode *
activate_scheduled_race_object(RacerEntity *object)
{
  object->status_flags = (unsigned char)(
    object->status_flags | SCHEDULED_RACE_OBJECT_RETAINED);
  object->status_flags = (unsigned char)(
    object->status_flags &
    (unsigned char)~SCHEDULED_RACE_OBJECT_PENDING_POOL_RETURN);

  return insert_intrusive_list_node_by_int_key(
    race_object_scheduler.object_list,
    &object->list_node.links,
    offsetof(RacerEntity, position_z), 1);
}
