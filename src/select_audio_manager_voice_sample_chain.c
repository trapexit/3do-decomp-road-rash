#include "audio_manager_voice_internal.h"

int
select_audio_manager_voice_sample_chain(int  voice_id,
                                        int *sample_items,
                                        int  sample_count,
                                        int  sample_mode)
{
  AudioManagerVoice *voice;
  int result;

  voice = resolve_audio_manager_voice(voice_id);
  if(voice == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }
  if(voice->is_owned == 0)
    {
      return AUDIO_MANAGER_INVALID_VOICE_ERROR;
    }

  result = replace_audio_manager_voice_samples(
    voice, sample_items, sample_count,
    sample_mode & AUDIO_MANAGER_BYTE_MASK);
  if(result >= 0)
    {
      return enable_audio_manager_voice_sample_linking(voice, 0);
    }
  return result;
}
