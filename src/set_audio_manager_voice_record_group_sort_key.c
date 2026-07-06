#include "audio_manager_voice_internal.h"

int
set_audio_manager_voice_record_group_sort_key(AudioManagerVoicePrefix *voice,
                                              int                      sort_key)
{
  voice->group_sort_key = sort_key;
  return 0;
}
