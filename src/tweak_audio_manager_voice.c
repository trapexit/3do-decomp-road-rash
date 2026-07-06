#include "audio_manager_voice_internal.h"

int
set_audio_voice_frequency_and_amplitude(int *voice,
                                        int  frequency,
                                        int  amplitude);

int
tweak_audio_manager_voice(int voice_item,
                          int frequency,
                          int amplitude,
                          int pan)
{
  AudioManagerVoice *voice;
  int result;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  if(pan >= 0)
    {
      result = set_audio_manager_voice_record_pan(voice, pan);
      if(result != 0)
        {
          return result;
        }
    }

  return set_audio_voice_frequency_and_amplitude(
    (int *)voice, frequency, amplitude);
}
