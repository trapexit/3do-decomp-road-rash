#include "audio.h"

#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

int
is_audio_manager_object_voice_available_for_sort_key(AudioManagerLoadedObject *audio_object,
                                                     int                       voice_index,
                                                     int                       requested_sort_key)
{
  int is_available;
  int current_sort_key;
  AudioManagerVoice *voice;
  int result;
  int current_value;
  int requested_value;

  is_available = 0;
  current_sort_key = 0;
  if(audio_object != 0 && voice_index >= 0 &&
     voice_index < audio_object->voice_count)
    {
      voice = audio_object->voices[voice_index];
      if(voice != 0 && voice->is_owned == 0)
        {
          result = get_audio_manager_voice_record_group_sort_key(
            voice, &current_sort_key);
          if(result < 0)
            {
              music_library_globals.last_error = result;
            }

          current_value = current_sort_key & AUDIO_MANAGER_BYTE_MASK;
          requested_value = requested_sort_key & AUDIO_MANAGER_BYTE_MASK;
          if(get_audio_manager_voice_status(voice) != AF_STARTED)
            {
              is_available = 1;
            }
          else
            {
              if((requested_sort_key &
                  AUDIO_MANAGER_SORT_KEY_OPTION_MASK) != 0)
                {
                  if(current_value < requested_value)
                    {
                      is_available = 1;
                    }
                }
              if(current_value <= requested_value)
                {
                  is_available = 1;
                }
            }
        }
    }

  return is_available;
}
