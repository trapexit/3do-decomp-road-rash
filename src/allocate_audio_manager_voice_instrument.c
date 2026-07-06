#include "audio_manager_voice_internal.h"

int
allocate_audio_manager_voice_instrument(int voice_item,
                                        int lock_state_on_failure)
{
  AudioManagerVoice *voice;
  int result;
  unsigned char failure_lock_state;

  failure_lock_state = (unsigned char)lock_state_on_failure;
  result = 0;
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
          voice->instrument_lock_state = failure_lock_state;
        }
      else
        {
          result = 1;
        }
    }
  return result;
}
