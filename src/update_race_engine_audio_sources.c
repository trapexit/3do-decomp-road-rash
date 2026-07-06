#include "audio_manager_voice.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RaceEngineAudioVoiceIndex
{
  PLAYER_ENGINE_VOICE = 0,
  NEAREST_ENGINE_VOICE = 1,
  SECOND_ENGINE_VOICE = 2
};

static
void
update_object_engine_audio(RacerEntity    *object,
                           AudioVoiceSlot *voice)
{
  if(object != 0)
    {
      update_race_object_engine_audio(object, voice);
    }
  else
    {
      stop_race_audio_voice(voice->voice_item);
    }
}


void
update_race_engine_audio_sources(void)
{
  RacerEntity *nearest_object;
  RacerEntity *second_nearest_object;

  update_race_object_engine_audio(
    (RacerEntity *)race_rider_state.player_object,
    &game_audio_state.engine_voices[PLAYER_ENGINE_VOICE]);
  if(race_mode_state.engine_audio_enabled == 0)
    {
      return;
    }

  find_nearest_engine_audio_objects(
    &nearest_object, &second_nearest_object);
  update_object_engine_audio(
    nearest_object,
    &game_audio_state.engine_voices[NEAREST_ENGINE_VOICE]);
  update_object_engine_audio(
    second_nearest_object,
    &game_audio_state.engine_voices[SECOND_ENGINE_VOICE]);
}
