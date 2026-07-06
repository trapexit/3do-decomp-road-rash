#include "audio_manager_voice_internal.h"

enum AudioManagerVoiceSampleConstant
{
  AUDIO_MANAGER_SAMPLE_MODE_MASK = 0xFF,
  AUDIO_MANAGER_BAD_SAMPLE_ERROR = -21
};

int
replace_audio_manager_voice_samples(AudioManagerVoice *voice,
                                    int               *sample_items,
                                    int                sample_count,
                                    int                sample_mode)
{
  int sample_index;
  int result;
  int masked_mode;
  int sample_item;

  masked_mode = sample_mode & AUDIO_MANAGER_SAMPLE_MODE_MASK;
  result = 0;
  clear_audio_manager_voice_samples(voice);

  if(sample_items == 0)
    {
      return result;
    }
  if(sample_items[0] < 0 || sample_count < 1)
    {
      return result;
    }

  sample_index = 1;
  do
    {
      sample_item = *sample_items++;
      if(sample_item < 0)
        {
          result = AUDIO_MANAGER_BAD_SAMPLE_ERROR;
          break;
        }
      if(sample_index >= sample_count)
        {
          sample_item = append_audio_manager_voice_sample(
            voice, sample_item, masked_mode);
        }
      else
        {
          sample_item = append_audio_manager_voice_sample(
            voice, sample_item, 0);
        }
      if(sample_item >= 0)
        {
          result = 0;
        }
      else
        {
          result = sample_item;
          clear_audio_manager_voice_samples(voice);
          return result;
        }
      sample_index++;
    } while(sample_index <= sample_count);

  return result;
}
