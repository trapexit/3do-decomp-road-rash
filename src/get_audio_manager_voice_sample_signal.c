#include "audio.h"
#include "audio_manager_voice_internal.h"
#include "item.h"

enum AudioManagerSampleSignalConstant
{
  AUDIO_MANAGER_CUE_NODE_TYPE = 0x405,
  AUDIO_MANAGER_ATTACHMENT_END_POSITION = -2
};

int
get_audio_manager_voice_sample_signal(int voice_item,
                                      int sample_selector)
{
  AudioManagerVoice *voice;
  AudioManagerVoiceSample *sample;
  int cue;
  int signal;

  voice = resolve_audio_manager_voice(voice_item);
  if(voice == 0 || voice->is_owned == 0)
    {
      return 0;
    }

  sample = &voice->first_sample;
  while(sample_selector > 0)
    {
      if(sample == 0)
        {
          return 0;
        }
      if(sample->sample_item < 0)
        {
          break;
        }
      sample = sample->next;
    }

  if(sample == 0 ||
     sample->sample_item < 0 || sample->attachment_item < 0)
    {
      return 0;
    }

  cue = sample->cue_item;
  if(cue < 0)
    {
      cue = CreateSizedItem(AUDIO_MANAGER_CUE_NODE_TYPE, 0, 0);
      sample->cue_item = cue;
      if(cue < 0)
        {
          return 0;
        }
    }

  signal = GetCueSignal(cue);
  if(signal == 0)
    {
      return 0;
    }
  if(MonitorAttachment(
       sample->attachment_item, cue,
       AUDIO_MANAGER_ATTACHMENT_END_POSITION) < 0)
    {
      return 0;
    }
  return signal;
}
