#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

AudioManagerLoadedObject *
get_audio_manager_object_by_index(int object_index)
{
  AudioManagerLoadedObject *audio_object;

  audio_object = 0;
  if(object_index >= 0 &&
     object_index < music_library_globals.loaded_audio_object_count)
    {
      audio_object = (AudioManagerLoadedObject *)
                     ((void * volatile *)gLoadedAudioObjects)[object_index];
    }
  return audio_object;
}
