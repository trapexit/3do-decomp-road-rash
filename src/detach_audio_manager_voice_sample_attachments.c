#include "audio.h"
#include "audio_manager_voice_internal.h"
#include "item.h"

void
detach_audio_manager_voice_sample_attachments(AudioManagerVoice *voice)
{
  AudioManagerVoiceSample *sample;

  if(voice == 0)
    {
      return;
    }

  stop_audio_manager_voice_record(voice);
  sample = &voice->first_sample;
  if(sample == 0)
    {
      return;
    }

  do
    {
      if(sample->attachment_item >= 0)
        {
          DetachSample(sample->attachment_item);
          sample->attachment_item = -1;
        }
      if(sample->cue_item >= 0)
        {
          DeleteItem(sample->cue_item);
          sample->cue_item = -1;
        }
      sample = sample->next;
    } while(sample != 0);
}
