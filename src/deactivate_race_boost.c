#include "racer_runtime.h"
#include "spatial_audio_event_queue.h"

enum RaceBoostDeactivationConstant
{
  RACE_BOOST_ACTIVE_FLAG = 0x20,
  RACE_BOOST_DEACTIVATION_AUDIO_EVENT = 0x1C
};

void
deactivate_race_boost(RacerEntity *rider)
{
  if((rider->status_flags & RACE_BOOST_ACTIVE_FLAG) == 0)
    {
      return;
    }

  queue_spatial_audio_event(
    RACE_BOOST_DEACTIVATION_AUDIO_EVENT,
    &rider->position_z, &rider->velocity.forward);
  rider->status_flags = (unsigned char)(
    rider->status_flags & (unsigned char)~RACE_BOOST_ACTIVE_FLAG);
}
