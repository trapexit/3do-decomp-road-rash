#include "audio_manager_voice_internal.h"

int
stop_and_clear_audio_manager_loaded_object_voices(AudioManagerLoadedObject *audio_object)
{
  int voice_index;
  int voice_count;
  AudioManagerVoice *voice;

  voice_index = 0;
  if(audio_object == 0)
    {
      return 0;
    }

  voice_count = audio_object->voice_count;
  if(voice_count <= 0)
    {
      return voice_count;
    }

  do
    {
      voice = audio_object->voices[voice_index];
      if(voice != 0)
        {
          stop_audio_manager_voice_record(voice);
          replace_audio_manager_voice_samples(
            voice, 0, 0,
            AUDIO_MANAGER_STOP_INSTRUMENT_AT_SAMPLE_END);
        }
      voice_index++;
      voice_count = audio_object->voice_count;
    } while(voice_index < voice_count);

  return voice_count;
}
