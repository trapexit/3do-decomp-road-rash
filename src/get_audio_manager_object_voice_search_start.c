#include "audio_manager_voice_internal.h"

int
get_audio_manager_object_voice_search_start(AudioManagerLoadedObject *audio_object)
{
  if(audio_object != 0)
    {
      return audio_object->voice_search_start;
    }
  return 0;
}
