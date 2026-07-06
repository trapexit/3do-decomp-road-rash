#include "audio_manager_voice_internal.h"

AudioManagerVoice *
claim_audio_manager_object_voice(AudioManagerLoadedObject *audio_object,
                                 int                      *voice_id)
{
  AudioManagerVoice *voice;
  AudioManagerVoicePrefix *voice_state;
  int voice_index;
  int first_voice_index;
  int voice_count;

  if(voice_id != 0)
    {
      *voice_id = -1;
    }

  voice_index = audio_object->voice_search_start;
  first_voice_index = voice_index;
  voice = audio_object->voices[voice_index];
  if(voice == 0)
    {
      return 0;
    }

  voice_state = (AudioManagerVoicePrefix *)voice;
  if(voice_state->is_owned != 0)
    {
      voice_count = audio_object->voice_count;
      do
        {
          voice_index = (voice_index + 1) % voice_count;
          voice = audio_object->voices[voice_index];
          if(voice == 0)
            {
              return 0;
            }
          voice_state = (AudioManagerVoicePrefix *)voice;
        } while(
        voice_state->is_owned != 0 &&
        voice_index != first_voice_index);

      if(voice_index == first_voice_index)
        {
          return 0;
        }
    }

  stop_audio_manager_voice_record(voice);
  voice_state->is_owned = 1;

  if(voice_id != 0)
    {
      *voice_id = voice_index |
                  (audio_object->object_key << AUDIO_MANAGER_VOICE_KEY_SHIFT);
    }

  return voice;
}
