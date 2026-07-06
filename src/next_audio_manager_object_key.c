#include "audio_manager_collection_runtime.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

int
next_audio_manager_object_key(void)
{
  AudioManagerLoadedObject *audio_object;
  int count;
  int index;
  int next_key;

  next_key = AUDIO_MANAGER_FIRST_DYNAMIC_OBJECT_KEY;
  index = 0;
  count = music_library_globals.loaded_audio_object_count;
  if(count <= 0)
    {
      return next_key;
    }

  do
    {
      audio_object = (AudioManagerLoadedObject *)gLoadedAudioObjects[index];
      if(audio_object->object_key >= next_key)
        {
          next_key = audio_object->object_key + 1;
        }
      index++;
    } while(index < count);

  return next_key;
}
