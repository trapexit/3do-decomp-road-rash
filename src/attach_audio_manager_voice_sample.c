#include "audio.h"
#include "audio_manager_voice_internal.h"

int
create_audio_attachment(int instrument_item,
                        int sample_item,
                        int hook_name,
                        int flags);

int
attach_audio_manager_voice_sample(AudioManagerVoice       *voice,
                                  AudioManagerVoiceSample *sample)
{
  int result;

  result = 0;
  if(sample->sample_item >= 0)
    {
      if((sample->attachment_flags &
          AUDIO_MANAGER_VOICE_SAMPLE_STOP_AT_END_FLAG) != 0)
        {
          result = create_audio_attachment(
            voice->instrument_item, sample->sample_item,
            0, AF_ATTF_FATLADYSINGS);
        }
      else
        {
          result = AttachSample(
            voice->instrument_item, sample->sample_item, 0);
        }
      sample->attachment_item = result;
    }

  return result;
}
