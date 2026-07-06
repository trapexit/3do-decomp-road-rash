#include "audio_manager_voice_internal.h"

AudioManagerVoiceSample *
get_audio_voice_sample_by_index(AudioManagerVoice *voice,
                                int                sample_index)
{
  AudioManagerVoiceSample *sample;

  if(voice == 0)
    {
      return 0;
    }
  sample = &voice->first_sample;
  while(sample != 0 && sample_index-- > 0)
    {
      sample = sample->next;
    }
  return sample;
}
