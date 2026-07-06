#include "audio.h"
#include "operamath.h"
#include "string.h"

enum AudioManagerSamplePlayerConstant
{
  AUDIO_MANAGER_SAMPLE_TAG_CHANNELS = 0,
  AUDIO_MANAGER_SAMPLE_TAG_COMPRESSION = 1,
  AUDIO_MANAGER_SAMPLE_TAG_BITS = 2,
  AUDIO_MANAGER_SAMPLE_TAG_RATE = 3,
  AUDIO_MANAGER_SAMPLE_TAG_END = 4,
  AUDIO_MANAGER_SAMPLE_TAG_COUNT = 5,
  AUDIO_MANAGER_STEREO_CHANNEL_COUNT = 2,
  AUDIO_MANAGER_HALF_SAMPLE_RATE_HZ = 11025,
  AUDIO_MANAGER_EIGHT_BIT_SAMPLE = 8,
  AUDIO_MANAGER_SIXTEEN_BIT_SAMPLE = 16
};

/*
 * This is the Audio Manager's output-buffer adapter for the SDK 1.3
 * SelectSamplePlayer selection matrix.  Its failure and empty-string
 * behavior is part of the original three-argument contract.
 */
char *
select_audio_manager_sample_player_name(int   sample_item,
                                        int   variable_pitch,
                                        char *instrument_name)
{
  TagArg tags[AUDIO_MANAGER_SAMPLE_TAG_COUNT];
  const char *player_name;
  int channel_count;
  int compression_type;
  int bits_per_sample;
  int sample_rate;
  int half_sample_rate;

  player_name = 0;
  if(sample_item < 0 || instrument_name == 0)
    {
      return 0;
    }

  tags[AUDIO_MANAGER_SAMPLE_TAG_CHANNELS].ta_Tag = AF_TAG_CHANNELS;
  tags[AUDIO_MANAGER_SAMPLE_TAG_COMPRESSION].ta_Tag =
    AF_TAG_COMPRESSIONTYPE;
  tags[AUDIO_MANAGER_SAMPLE_TAG_BITS].ta_Tag = AF_TAG_NUMBITS;
  tags[AUDIO_MANAGER_SAMPLE_TAG_RATE].ta_Tag = AF_TAG_SAMPLE_RATE;
  tags[AUDIO_MANAGER_SAMPLE_TAG_END].ta_Tag = TAG_END;

  if(GetAudioItemInfo(sample_item, tags) < 0)
    {
      return 0;
    }

  channel_count =
    (int)tags[AUDIO_MANAGER_SAMPLE_TAG_CHANNELS].ta_Arg;
  compression_type =
    (int)tags[AUDIO_MANAGER_SAMPLE_TAG_COMPRESSION].ta_Arg;
  bits_per_sample =
    (int)tags[AUDIO_MANAGER_SAMPLE_TAG_BITS].ta_Arg;
  sample_rate = ConvertF16_32(
    (int)tags[AUDIO_MANAGER_SAMPLE_TAG_RATE].ta_Arg);
  half_sample_rate =
    (((sample_rate + 1) >> 1) == AUDIO_MANAGER_HALF_SAMPLE_RATE_HZ);

  if(variable_pitch != 0)
    {
      if(channel_count != 1 || compression_type != 0)
        {
          return 0;
        }
      if(bits_per_sample == AUDIO_MANAGER_SIXTEEN_BIT_SAMPLE)
        {
          player_name = "sampler.dsp";
        }
      else
        {
          player_name = "varmono8.dsp";
        }
    }
  else if(channel_count == 1)
    {
      if(compression_type == ID_SDX2)
        {
          if(half_sample_rate)
            {
              player_name = "dcsqxdhalfmono.dsp";
            }
          else
            {
              player_name = "dcsqxdmono.dsp";
            }
        }
      else if(compression_type == ID_ADP4)
        {
          if(half_sample_rate)
            {
              player_name = "adpcmhalfmono.dsp";
            }
          else
            {
              player_name = "adpcmmono.dsp";
            }
        }
      else if(bits_per_sample == AUDIO_MANAGER_EIGHT_BIT_SAMPLE)
        {
          if(half_sample_rate)
            {
              player_name = "halfmono8.dsp";
            }
          else
            {
              player_name = "fixedmono8.dsp";
            }
        }
      else if(half_sample_rate)
        {
          player_name = "halfmonosample.dsp";
        }
      else
        {
          player_name = "fixedmonosample.dsp";
        }
    }
  else if(channel_count == AUDIO_MANAGER_STEREO_CHANNEL_COUNT)
    {
      if(compression_type == ID_SDX2)
        {
          if(half_sample_rate)
            {
              player_name = "dcsqxdhalfstereo.dsp";
            }
          else
            {
              player_name = "dcsqxdstereo.dsp";
            }
        }
      else if(compression_type == ID_ADP4)
        {
          if(half_sample_rate)
            {
              player_name = "adpcmhalfstereo.dsp";
            }
          else
            {
              player_name = "adpcmstereo.dsp";
            }
        }
      else if(bits_per_sample == AUDIO_MANAGER_EIGHT_BIT_SAMPLE)
        {
          if(half_sample_rate)
            {
              player_name = "halfstereo8.dsp";
            }
          else
            {
              player_name = "fixedstereo8.dsp";
            }
        }
      else if(half_sample_rate)
        {
          player_name = "halfstereosample.dsp";
        }
      else
        {
          player_name = "fixedstereosample.dsp";
        }
    }

  if(player_name == 0)
    {
      memcpy(instrument_name, "", 1);
    }
  else
    {
      strcpy(instrument_name, player_name);
    }

  return instrument_name;
}
