#include "audio_manager_voice_internal.h"

int
disown_audio_manager_voice(int voice_id)
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
  if(voice_state->is_owned == 0)
    {
      return 0;
    }

  result = unlock_audio_manager_voice(voice_id);
  if(result < 0)
    {
      return result;
    }
  voice_state->is_owned = 0;
  return 0;
}
