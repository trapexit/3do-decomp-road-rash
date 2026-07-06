#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

enum RaceEngineAudioObjectClass
{
  RACE_ENGINE_AUDIO_CLASS_RIDER = 4,
  RACE_ENGINE_AUDIO_CLASS_TRAFFIC = 0x10
};

void
update_race_object_engine_audio(RacerEntity    *object,
                                AudioVoiceSlot *voice)
{
  update_racer_engine_audio_spatial_state(object);

  if(object->spatial_gain_scale_8_8 == 0)
    {
      return;
    }

  if(object->collision_class == RACE_ENGINE_AUDIO_CLASS_RIDER)
    {
      update_rider_engine_pitch(object);
      if(race_mode_state.engine_audio_enabled != 0)
        {
          update_rider_engine_audio_voice(object, voice);
        }
      return;
    }

  if(object->collision_class == RACE_ENGINE_AUDIO_CLASS_TRAFFIC)
    {
      update_traffic_engine_audio_voice(object, voice);
    }
}
