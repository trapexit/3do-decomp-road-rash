#include "audio.h"

#define AUDIO_REFERENCE_SAMPLE_RATE_F16 ((ufrac16)0xAC440000U)

enum
{
  AUDIO_INFO_TAG_WORD_COUNT = 4,
  AUDIO_INFO_VALUE_WORD = 1,
  AUDIO_INFO_END_WORD = 2
};

int
scale_audio_value_for_sample_rate(int sample_item,
                                  int value)
{
  int tags[AUDIO_INFO_TAG_WORD_COUNT];

  if(sample_item >= 0)
    {
      tags[0] = AF_TAG_SAMPLE_RATE;
      tags[AUDIO_INFO_VALUE_WORD] = 0;
      tags[AUDIO_INFO_END_WORD] = 0;

      if(GetAudioItemInfo(sample_item, (TagArg *)tags) >= 0)
        {
          value = (int)MulUF16(
            (ufrac16)value,
            DivUF16((ufrac16)tags[AUDIO_INFO_VALUE_WORD],
                    AUDIO_REFERENCE_SAMPLE_RATE_F16));
        }
    }

  return value;
}
