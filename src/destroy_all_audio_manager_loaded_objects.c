#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

void
destroy_all_audio_manager_loaded_objects(void)
{
  int index;
  int object_count;
  AudioManagerLoadedObject **objects;

  objects = (AudioManagerLoadedObject **)gLoadedAudioObjects;
  object_count = music_library_globals.loaded_audio_object_count;
  for(index = 0; index < object_count; index++)
    {
      destroy_audio_manager_loaded_object(objects[index]);
      objects[index] = 0;
    }

  music_library_globals.loaded_audio_object_count = 0;
}
