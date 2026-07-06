#include "audio.h"

#include "aiff_sample_metadata.h"

#define SAMPLE_TAG_CAPACITY 20

#define COMPRESSION_CODE_NONE 0x4E4F4E45
#define COMPRESSION_CODE_SDX2 0x53445832
#define COMPRESSION_CODE_SDX3 0x53445833
#define COMPRESSION_CODE_ADP4 0x41445034

#define COMPRESSION_RATIO_NONE 1
#define COMPRESSION_RATIO_SDX2 2
#define COMPRESSION_RATIO_SDX3 3
#define COMPRESSION_RATIO_ADP4 4

#define MUSIC_ERROR_BAD_SAMPLE_INFO (-21)

typedef struct AudioTagValue
{
  int tag;
  int value;
} AudioTagValue;

int
create_sample_from_metadata(AiffSampleMetadata *metadata)
{
  AudioTagValue tags[SAMPLE_TAG_CAPACITY];
  int index;
  int compression_type;

  if(metadata == 0)
    {
      return MUSIC_ERROR_BAD_SAMPLE_INFO;
    }

  index = 0;
  tags[index].tag = AF_TAG_CHANNELS;
  tags[index].value = metadata->channel_count;
  index++;
  tags[index].tag = AF_TAG_FRAMES;
  tags[index].value = metadata->frame_count;
  index++;
  tags[index].tag = AF_TAG_NUMBITS;
  tags[index].value = metadata->bits_per_sample;
  index++;
  tags[index].tag = AF_TAG_SAMPLE_RATE;
  tags[index].value = metadata->sample_rate;
  index++;
  tags[index].tag = AF_TAG_COMPRESSIONTYPE;
  tags[index].value = metadata->compression_type;
  index++;
  tags[index].tag = AF_TAG_COMPRESSIONRATIO;

  compression_type = metadata->compression_type;
  if(compression_type == 0 || compression_type == COMPRESSION_CODE_NONE)
    {
      tags[index].value = COMPRESSION_RATIO_NONE;
    }
  else if(compression_type == COMPRESSION_CODE_SDX2)
    {
      tags[index].value = COMPRESSION_RATIO_SDX2;
    }
  else if(compression_type == COMPRESSION_CODE_SDX3)
    {
      tags[index].value = COMPRESSION_RATIO_SDX3;
    }
  else if(compression_type == COMPRESSION_CODE_ADP4)
    {
      tags[index].value = COMPRESSION_RATIO_ADP4;
    }
  else
    {
      tags[index].value = COMPRESSION_RATIO_NONE;
    }
  index++;

  tags[index].tag = AF_TAG_BASENOTE;
  tags[index].value = metadata->base_note;
  index++;
  tags[index].tag = AF_TAG_DETUNE;
  tags[index].value = metadata->detune;
  index++;
  tags[index].tag = AF_TAG_LOWNOTE;
  tags[index].value = metadata->low_note;
  index++;
  tags[index].tag = AF_TAG_HIGHNOTE;
  tags[index].value = metadata->high_note;
  index++;
  tags[index].tag = AF_TAG_LOWVELOCITY;
  tags[index].value = metadata->low_velocity;
  index++;
  tags[index].tag = AF_TAG_HIGHVELOCITY;
  tags[index].value = metadata->high_velocity;
  index++;

  if(metadata->sustain_loop_type > 0)
    {
      tags[index].tag = AF_TAG_SUSTAINBEGIN;
      tags[index].value = metadata->sustain_begin;
      index++;
      tags[index].tag = AF_TAG_SUSTAINEND;
      tags[index].value = metadata->sustain_end;
      index++;
    }

  if(metadata->release_loop_type > 0)
    {
      tags[index].tag = AF_TAG_RELEASEBEGIN;
      tags[index].value = metadata->release_begin;
      index++;
      tags[index].tag = AF_TAG_RELEASEEND;
      tags[index].value = metadata->release_end;
      index++;
    }

  tags[index].tag = TAG_END;
  tags[index].value = 0;
  return CreateSample((TagArg *)tags);
}


#undef SAMPLE_TAG_CAPACITY
#undef COMPRESSION_CODE_NONE
#undef COMPRESSION_CODE_SDX2
#undef COMPRESSION_CODE_SDX3
#undef COMPRESSION_CODE_ADP4
#undef COMPRESSION_RATIO_NONE
#undef COMPRESSION_RATIO_SDX2
#undef COMPRESSION_RATIO_SDX3
#undef COMPRESSION_RATIO_ADP4
#undef MUSIC_ERROR_BAD_SAMPLE_INFO
