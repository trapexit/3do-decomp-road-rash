#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

int
stop_and_clear_all_audio_manager_loaded_object_voices(void)
{
  int object_index;
  int object_count;
  AudioManagerLoadedObject **audio_objects;

  object_index = 0;
  object_count = music_library_globals.loaded_audio_object_count;
  if(object_count <= 0)
    {
      return object_count;
    }

  audio_objects = (AudioManagerLoadedObject **)gLoadedAudioObjects;
  do
    {
      stop_and_clear_audio_manager_loaded_object_voices(
        audio_objects[object_index]);
      object_index++;
      object_count = music_library_globals.loaded_audio_object_count;
    } while(object_index < object_count);

  return object_count;
}
