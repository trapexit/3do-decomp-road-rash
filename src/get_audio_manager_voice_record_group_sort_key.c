#include "audio_manager_voice_internal.h"

int
get_audio_manager_voice_record_group_sort_key(AudioManagerVoicePrefix *voice,
                                              int                     *sort_key)
{
  if(voice == 0 || sort_key == 0)
    {
      return AUDIO_MANAGER_BAD_OBJECT_ERROR;
    }
  *sort_key = voice->group_sort_key;
  return 0;
}
