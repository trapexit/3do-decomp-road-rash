#include "audio_manager_voice_internal.h"

int
attach_all_audio_manager_voice_samples(AudioManagerVoice *voice)
{
  AudioManagerVoiceSample *sample;
  int result;

  sample = &voice->first_sample;
  if(sample == 0)
    {
      return 0;
    }

  do
    {
      result = attach_audio_manager_voice_sample(voice, sample);
      if(result < 0)
        {
          return result;
        }
      sample = sample->next;
    } while(sample != 0);

  return result;
}
