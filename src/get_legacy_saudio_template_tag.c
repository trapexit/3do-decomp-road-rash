#include "saudio_subscriber_runtime.h"

long
get_legacy_saudio_template_tag(SAudioSampleDescriptor *descriptor)
{
  long channel_count;
  unsigned int compression_type;
  long sample_rate;
  long sample_size;

  channel_count = descriptor->channel_count;
  compression_type = descriptor->compression_type;
  sample_rate = descriptor->sample_rate;
  sample_size = descriptor->sample_size;

  if(channel_count == SAUDIO_MONO_CHANNEL_COUNT)
    {
      if(compression_type == SAUDIO_COMPRESSION_SDX2)
        {
          if(sample_rate == SAUDIO_SAMPLE_RATE_22KHZ)
            {
              return SAUDIO_TEMPLATE_TAG_22KHZ_16BIT_MONO_SDX2;
            }
          return SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_MONO_SDX2;
        }
      if(sample_size == SAUDIO_SAMPLE_WIDTH_8_BITS)
        {
          if(sample_rate == SAUDIO_SAMPLE_RATE_22KHZ)
            {
              return SAUDIO_TEMPLATE_TAG_22KHZ_8BIT_MONO;
            }
          return SAUDIO_TEMPLATE_TAG_44KHZ_8BIT_MONO;
        }
      if(sample_rate == SAUDIO_SAMPLE_RATE_44KHZ)
        {
          return SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_MONO;
        }
      return -1;
    }

  if(channel_count == SAUDIO_STEREO_CHANNEL_COUNT)
    {
      if(compression_type == SAUDIO_COMPRESSION_SDX2)
        {
          if(sample_rate == SAUDIO_SAMPLE_RATE_22KHZ)
            {
              return SAUDIO_TEMPLATE_TAG_22KHZ_16BIT_STEREO_SDX2;
            }
          return SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_STEREO_SDX2;
        }
      if(sample_size == SAUDIO_SAMPLE_WIDTH_8_BITS)
        {
          if(sample_rate == SAUDIO_SAMPLE_RATE_22KHZ)
            {
              return SAUDIO_TEMPLATE_TAG_22KHZ_8BIT_STEREO;
            }
          return SAUDIO_TEMPLATE_TAG_44KHZ_8BIT_STEREO;
        }
      if(sample_rate == SAUDIO_SAMPLE_RATE_44KHZ)
        {
          return SAUDIO_TEMPLATE_TAG_44KHZ_16BIT_STEREO;
        }
    }

  return -1;
}
