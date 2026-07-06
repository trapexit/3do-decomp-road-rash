#include "audio.h"
#include "audio_manager_voice_internal.h"

int
link_audio_manager_voice_sample_attachments(AudioManagerVoice *voice)
{
  AudioManagerVoiceSample *first_sample;
  AudioManagerVoiceSample *sample;
  int wrap_to_first;
  int result;

  result = 0;
  if((voice->sample_chain_flags &
      AUDIO_MANAGER_VOICE_SAMPLE_LINK_FLAG) == 0)
    {
      return 0;
    }

  wrap_to_first =
    (voice->sample_chain_flags &
     AUDIO_MANAGER_VOICE_SAMPLE_WRAP_FLAG) != 0;
  first_sample = &voice->first_sample;
  sample = first_sample;

  while(sample != 0)
    {
      if(sample->attachment_item < 0)
        {
          return result;
        }

      if(sample->next != 0)
        {
          result = LinkAttachments(
            sample->attachment_item, sample->next->attachment_item);
        }
      else if(wrap_to_first)
        {
          result = LinkAttachments(
            sample->attachment_item, first_sample->attachment_item);
        }
      else
        {
          result = LinkAttachments(sample->attachment_item, 0);
        }

      if(result < 0)
        {
          return result;
        }
      sample = sample->next;
    }

  return result;
}
