#include "audio_manager_voice.h"
#include "audio_manager_voice_internal.h"
#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

static
void
destroy_released_race_voice_instrument(int voice_id)
{
  AudioManagerVoice *voice;

  if(voice_id < 0)
    {
      return;
    }
  voice = resolve_audio_manager_voice(voice_id);
  if(voice == 0)
    {
      return;
    }
  destroy_audio_manager_voice_instrument(voice);
}

int
release_race_engine_voices(void)
{
  int result;
  int release_result;
  int voice_index;

  result = 0;
  for(voice_index = 0;
      voice_index < RACE_AUDIO_ENGINE_VOICE_COUNT;
      voice_index++)
    {
      if(game_audio_state.engine_voices[voice_index].voice_item >= 0)
        {
          release_result = disown_audio_manager_sample_voice(
            game_audio_state.engine_voices[voice_index].voice_item);
          if(release_result >= 0)
            {
              destroy_released_race_voice_instrument(
                game_audio_state.engine_voices[voice_index].voice_item);
              game_audio_state.engine_voices[voice_index].voice_item =
                RACE_AUDIO_UNASSIGNED_ITEM;
            }
          else if(result >= 0)
            {
              result = release_result;
            }
        }
    }

  return result;
}
