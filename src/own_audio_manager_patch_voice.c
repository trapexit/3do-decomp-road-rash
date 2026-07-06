#include "audio_manager_voice_internal.h"

int
own_audio_manager_patch_voice(const char *instrument_name)
{
  AudioManagerLoadedObject *audio_object;
  int voice_id;
  int object_index;

  voice_id = -1;
  object_index = 1;
  audio_object = get_audio_manager_object_by_index(0);
  while(audio_object != 0)
    {
      if(audio_object->lookup_flags == 0 &&
         audio_manager_strings_equal_ignore_case(
           audio_object->instrument_name, instrument_name) != 0 &&
         claim_audio_manager_object_voice(audio_object, &voice_id) != 0)
        {
          return voice_id;
        }
      audio_object = get_audio_manager_object_by_index(object_index);
      object_index++;
    }

  return AUDIO_MANAGER_VOICE_UNAVAILABLE_ERROR;
}
