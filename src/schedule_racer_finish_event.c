
#include "bss_early_data.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

int
record_racer_finish_result(RacerEntity *racer,
                           int          finish_tick);

enum RacerFinishScheduleConstant
{
  RACER_FINISH_SCHEDULED_FLAG = 0x10
};

int
schedule_racer_finish_event(RacerEntity *racer)
{
  RacerEntity *owner;
  RacerEntity *challenge;
  unsigned char flags;
  int finish_tick;
  PendingRacerEvent *slot;

  owner = racer->owner;
  flags = racer->status_flags;
  if((flags & RACER_FINISH_SCHEDULED_FLAG) != 0)
    {
      return 0;
    }

  racer->status_flags = flags | RACER_FINISH_SCHEDULED_FLAG;
  owner->status_flags |= RACER_FINISH_SCHEDULED_FLAG;

  challenge = (RacerEntity *)race_rider_state.challenge_object;
  if(challenge != 0)
    {
      if(racer == challenge->paired_state)
        {
          return 0;
        }
    }

  finish_tick = frame_tick;
  slot = &gPendingRacerEvents[
    race_object_scheduler.pending_racer_count];
  slot->trigger_time = finish_tick;
  slot->racer = (unsigned int)racer;

  return record_racer_finish_result(racer, finish_tick);
}
