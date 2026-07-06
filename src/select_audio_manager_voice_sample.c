#include "audio_manager_voice_internal.h"

int
select_audio_manager_voice_sample(int voice_item,
                                  int sample_item,
                                  int sample_mode)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  if(voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  return replace_audio_manager_voice_samples(
    voice, &sample_item, 1,
    sample_mode & AUDIO_MANAGER_BYTE_MASK);
}
