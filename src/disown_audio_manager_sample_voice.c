#include "audio_manager_voice_internal.h"

int
disown_audio_manager_sample_voice(int voice_id)
{
  AudioManagerVoice *voice;
  AudioManagerVoicePrefix *voice_state;
  int result;

  voice = resolve_audio_manager_voice(voice_id);
  if(voice == 0)
    {
      return 0;
    }

  voice_state = (AudioManagerVoicePrefix *)voice;
  if(voice->instrument_item >= 0)
    {
      result = stop_audio_manager_voice_record(voice);
      if(result < 0)
        {
          return result;
        }
    }
  result = replace_audio_manager_voice_samples(voice, 0, 0, 1);
  if(result < 0)
    {
      return result;
    }
  voice_state->is_owned = 0;

  return 0;
}
