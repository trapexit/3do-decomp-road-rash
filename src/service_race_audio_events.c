#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RaceAudioServiceConstants
{
  RACE_AUDIO_UPDATE_TICK = 1
};

int
service_race_audio_events(void)
{
  unsigned int flags;

  if(game_audio_state.initialized == 0)
    {
      return 0;
    }

  flags =
    track_simulation_globals.motion_source.periodic_event_flags.value;
  if((flags & RACE_AUDIO_UPDATE_TICK) == 0)
    {
      return (int)flags;
    }

  update_player_rider_audio_events();
  update_race_engine_audio_sources();
  return process_race_audio_event_queue();
}
