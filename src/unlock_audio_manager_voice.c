#include "audio_manager_voice_internal.h"

int
unlock_audio_manager_voice(int voice_item)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  voice->instrument_lock_state = 0;
  return 0;
}
