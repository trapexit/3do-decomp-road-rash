#include "audio_manager_voice_internal.h"

int
get_audio_manager_voice_instrument_item(int voice_item)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice != 0 && voice->is_owned != 0)
    {
      return voice->instrument_item;
    }
  return AUDIO_MANAGER_INVALID_VOICE_ERROR;
}
