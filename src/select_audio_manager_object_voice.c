#include "audio_manager_voice_internal.h"

AudioManagerVoice *
select_audio_manager_object_voice(AudioManagerLoadedObject *audio_object,
                                  int                       requested_sort_key,
                                  int                      *voice_index_out)
{
  int start_index;
  int voice_index;
  AudioManagerVoice *voice;

  voice = 0;
  if(audio_object != 0)
    {
      start_index = get_audio_manager_object_voice_search_start(
        audio_object);
      if(is_audio_manager_object_voice_available_for_sort_key(
           audio_object, start_index, requested_sort_key) != 0)
        {
          voice_index = start_index;
          voice = audio_object->voices[voice_index];
        }
      else
        {
          voice_index = advance_audio_manager_object_voice_search(
            audio_object);
          while(!is_audio_manager_object_voice_available_for_sort_key(
                  audio_object, voice_index, requested_sort_key) &&
                voice_index != start_index)
            {
              voice_index = advance_audio_manager_object_voice_search(
                audio_object);
            }

          voice = audio_object->voices[voice_index];
          if(voice->is_owned != 0)
            {
              voice = 0;
            }
        }

      if(voice != 0 && voice_index_out != 0)
        {
          *voice_index_out = voice_index;
        }
    }

  return voice;
}
