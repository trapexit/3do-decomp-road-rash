#include "ai_racer_runtime.h"
#include "race_object_lifecycle.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"

enum ChallengeRacerRemovalConstant
{
  CHALLENGE_RACER_REMOVAL_AUDIO_EVENT = 0x16
};

int
remove_challenge_racer_pair(AiRacerObject *unused_racer)
{
  AiRacerObject *challenge;

  (void)unused_racer;

  challenge = (AiRacerObject *)race_rider_state.challenge_object;
  if(challenge == 0)
    {
      return 0;
    }

  queue_spatial_audio_event(CHALLENGE_RACER_REMOVAL_AUDIO_EVENT, 0, 0);
  schedule_race_object_removal(challenge->paired_state);
  schedule_race_object_removal(challenge);
  race_rider_state.challenge_object = 0;
  return 0;
}
