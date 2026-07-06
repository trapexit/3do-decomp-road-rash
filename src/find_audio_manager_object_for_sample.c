#include "audio_manager_voice_internal.h"

AudioManagerLoadedObject *
find_audio_manager_object_for_sample(int sample_item)
{
  int object_key;

  object_key =
    find_compatible_audio_manager_object_key_for_sample(sample_item);
  if(object_key != 0)
    {
      return find_audio_manager_object_by_key(object_key);
    }

  return 0;
}
