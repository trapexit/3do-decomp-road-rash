#include "audio_manager_voice_internal.h"
#include "bss_late_data.h"
#include "rw_semantic_data.h"

enum AudioManagerSampleLookupConstant
{
  AUDIO_MANAGER_SAMPLE_NAME_CAPACITY = 256,
  AUDIO_MANAGER_SAMPLE_LOOKUP_PASS_COUNT = 2
};

int
find_compatible_audio_manager_object_key_for_sample(int sample_item)
{
  char instrument_name[AUDIO_MANAGER_SAMPLE_NAME_CAPACITY];
  AudioManagerLoadedObject *object;
  int object_key;
  int lookup_pass;
  int object_index;

  object_key = 0;
  for(lookup_pass = 0;
      lookup_pass < AUDIO_MANAGER_SAMPLE_LOOKUP_PASS_COUNT;
      lookup_pass++)
    {
      select_audio_manager_sample_player_name(
        sample_item, lookup_pass == 0, instrument_name);

      for(object_index = 0;
          object_index <
          music_library_globals.loaded_audio_object_count;
          object_index++)
        {
          object = (AudioManagerLoadedObject *)
                   gLoadedAudioObjects[object_index];
          if(object->lookup_flags == 0 &&
             audio_manager_strings_equal_ignore_case(
               object->instrument_name, instrument_name) != 0)
            {
              object_key = object->object_key;
            }
        }
    }

  return object_key;
}
