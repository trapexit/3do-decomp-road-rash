#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

AudioManagerLoadedObject *
find_audio_manager_object_by_key(int object_key)
{
  AudioManagerLoadedObject **objects;
  AudioManagerLoadedObject *object;
  int object_count;
  int object_index;

  object_count = music_library_globals.loaded_audio_object_count;
  objects = (AudioManagerLoadedObject **)gLoadedAudioObjects;
  for(object_index = 0; object_index < object_count; object_index++)
    {
      object = objects[object_index];
      if(object->object_key == object_key)
        {
          return object;
        }
    }

  return 0;
}
