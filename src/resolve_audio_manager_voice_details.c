#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

AudioManagerVoice *
resolve_audio_manager_voice_details(int                        voice_id,
                                    AudioManagerLoadedObject **audio_object,
                                    int                       *object_key,
                                    int                       *voice_index)
{
  AudioManagerLoadedObject *object;
  AudioManagerVoice *voice;
  int decoded_index;
  int decoded_key;

  decoded_index = 0;
  decoded_key = 0;
  voice = 0;

  decode_audio_manager_voice_id(
    voice_id, &decoded_key, &decoded_index);
  object = find_audio_manager_object_by_key(decoded_key);
  if(object != 0 && decoded_index >= 0 &&
     decoded_index < object->voice_count)
    {
      voice = object->voices[decoded_index];
      if(voice != 0)
        {
          if(audio_object != 0)
            {
              *audio_object = object;
            }
          if(object_key != 0)
            {
              *object_key = decoded_key;
            }
          if(voice_index != 0)
            {
              *voice_index = decoded_index;
            }
          return voice;
        }
    }

  decoded_key = 0;
  decoded_index = 0;
  music_library_globals.last_error = AUDIO_MANAGER_INVALID_VOICE_ERROR;
  if(audio_object != 0)
    {
      *audio_object = 0;
    }
  if(object_key != 0)
    {
      *object_key = decoded_key;
    }
  if(voice_index != 0)
    {
      *voice_index = decoded_index;
    }
  return 0;
}
