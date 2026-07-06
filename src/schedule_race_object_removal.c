#include "race_object_lifecycle.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

enum RaceObjectLifecycleValue
{
  RACE_OBJECT_RETAINED_FLAG = 0x04,
  RACE_OBJECT_POOL_ALLOCATION_FLAG = 0x08,
  RACE_OBJECT_REMOVAL_DELAY_TICKS = 100
};


void
schedule_race_object_removal(void *object)
{
  RacerEntity *race_object;
  int removal_tick;
  unsigned char flags;

  race_object = (RacerEntity *)object;

  flags = race_object->status_flags;
  flags &= (unsigned char)~RACE_OBJECT_RETAINED_FLAG;
  race_object->status_flags = flags;

  flags = race_object->status_flags;
  flags |= RACE_OBJECT_POOL_ALLOCATION_FLAG;
  race_object->status_flags = flags;

  removal_tick = frame_tick + RACE_OBJECT_REMOVAL_DELAY_TICKS;
  race_object->last_update_tick = removal_tick;
  race_object->next_update_tick = removal_tick;

  race_object_scheduler.removal_pending = 1;
}

