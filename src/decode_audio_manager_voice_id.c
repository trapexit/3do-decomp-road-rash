#include "audio_manager_voice_internal.h"

void
decode_audio_manager_voice_id(int  voice_id,
                              int *object_key,
                              int *voice_index)
{
  if(object_key != 0)
    {
      *object_key = voice_id >> AUDIO_MANAGER_VOICE_KEY_SHIFT;
    }
  if(voice_index != 0)
    {
      *voice_index = voice_id & AUDIO_MANAGER_VOICE_INDEX_MASK;
    }
}
