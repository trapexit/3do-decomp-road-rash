#include "audio_manager_voice_internal.h"

int
advance_audio_manager_object_voice_search(AudioManagerLoadedObject *audio_object)
{
  int voice_index;

  if(audio_object == 0)
    {
      return 0;
    }

  voice_index =
    (audio_object->voice_search_start + 1) % audio_object->voice_count;
  audio_object->voice_search_start = voice_index;
  return voice_index;
}
