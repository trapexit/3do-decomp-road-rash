#include "audio_manager_voice_internal.h"

int
lock_audio_manager_voice(int voice_item)
{
  AudioManagerVoice *voice;
  int result;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  if(voice->instrument_item < 0)
    {
      result = initialize_audio_manager_voice_instrument(voice);
      if(result < 0)
        {
          return result;
        }
    }

  voice->instrument_lock_state = 1;
  return voice->instrument_item;
}
