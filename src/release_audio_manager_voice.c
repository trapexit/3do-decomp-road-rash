#include "audio_manager_voice_internal.h"

int
release_audio_manager_voice(int voice_item)
{
  AudioManagerVoice *voice;
  int result;

  result = 0;
  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      result = AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  else
    {
      release_audio_manager_voice_record(voice);
    }

  return result;
}
