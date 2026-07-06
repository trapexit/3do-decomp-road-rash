#include "audio_manager_voice_internal.h"

int
add_audio_manager_voice_sample(int voice_id,
                               int sample_item,
                               int sample_mode)
{
  int result;

  result = append_audio_manager_voice_sample_by_id(
    voice_id, sample_item, sample_mode & AUDIO_MANAGER_BYTE_MASK);
  if(result >= 0)
    {
      result = 0;
    }
  return result;
}
