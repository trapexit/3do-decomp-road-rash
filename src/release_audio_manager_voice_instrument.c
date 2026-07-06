#include "audio_manager_voice_internal.h"

int
release_audio_manager_voice_instrument(int voice_item)
{
  AudioManagerVoice *voice;
  int result;

  result = 0;
  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  if(voice->instrument_item >= 0)
    {
      destroy_audio_manager_voice_instrument(voice);
      result = 1;
    }
  return result;
}
