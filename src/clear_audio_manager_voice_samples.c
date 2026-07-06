#include "audio_manager_voice_internal.h"
#include "rw_semantic_data.h"

void
clear_audio_manager_voice_samples(AudioManagerVoice *voice)
{
  AudioManagerVoiceSample *embedded_sample;
  AudioManagerVoiceSample *sample;
  AudioManagerVoiceSample *next;

  if(voice == 0)
    {
      return;
    }

  detach_audio_manager_voice_sample_attachments(voice);
  embedded_sample = &voice->first_sample;
  if(embedded_sample == 0)
    {
      return;
    }

  sample = embedded_sample;
  do
    {
      next = sample->next;
      if(sample == embedded_sample)
        {
          sample->sample_item = -1;
          sample->next = 0;
          sample->attachment_item = -1;
        }
      else
        {
          music_library_globals.release(
            (int)sample, sizeof(AudioManagerVoiceSample));
        }
      sample = next;
    } while(sample != 0);
}
