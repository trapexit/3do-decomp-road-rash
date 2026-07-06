#include "audio_manager_voice_internal.h"

int
set_audio_manager_voice_group_sort_key(int voice_id,
                                       int sort_key)
{
  AudioManagerVoice *voice;

  voice = resolve_audio_manager_voice(voice_id);
  if(voice != 0)
    {
      return set_audio_manager_voice_record_group_sort_key(
        (AudioManagerVoicePrefix *)voice, sort_key);
    }
  return AUDIO_MANAGER_BAD_OBJECT_ERROR;
}
