#include "aiff_sample_metadata.h"
#include "parse_aiff_sample_metadata_offsets.h"

#define AIFC_FORM_TYPE 0x41494643

#define AIFF_EXTENDED_RATE_WORDS 3
#define AIFF_RATE_WORD_HIGH 0
#define AIFF_RATE_WORD_MIDDLE 1
#define AIFF_RATE_WORD_LOW 2

#define COMM_RATE_WORD_HIGH 4
#define COMM_RATE_WORD_MIDDLE 5
#define COMM_RATE_WORD_LOW 6

#define MIDI_DEFAULT_BASE_NOTE 0x3C
#define MIDI_MAX_NOTE 0x7F
#define MIDI_MAX_VELOCITY 0x7F

#define READ_BE_S16(pointer, offset) \
        ((int)(short)((((unsigned char *)(pointer))[(offset)] << 8) | \
                      ((unsigned char *)(pointer))[(offset) + 1]))

#define READ_BE_U32(pointer, offset) \
        ((((unsigned char *)(pointer))[(offset)] << 24) | \
         (((unsigned char *)(pointer))[(offset) + 1] << 16) | \
         (((unsigned char *)(pointer))[(offset) + 2] << 8) | \
         ((unsigned char *)(pointer))[(offset) + 3])

int
decode_ieee_extended_sample_rate(unsigned char *encoded_rate);
int
find_aiff_marker_position_unaligned(unsigned char *marker_chunk,
                                    int            marker_id);

AiffSampleMetadata *
parse_aiff_sample_metadata(int                 form_type,
                           unsigned char      *common_chunk,
                           unsigned char      *marker_chunk,
                           unsigned char      *instrument_chunk,
                           AiffSampleMetadata *metadata)
{
  int extended_rate[AIFF_EXTENDED_RATE_WORDS];
  int loop_type;
  int loop_begin;
  int loop_end;

  if(metadata == 0)
    {
      return 0;
    }

  metadata->channel_count = 0;
  metadata->frame_count = 0;
  metadata->bits_per_sample = 0;
  metadata->sample_rate = 0;
  metadata->compression_type = 0;
  metadata->base_note = MIDI_DEFAULT_BASE_NOTE;
  metadata->detune = 0;
  metadata->low_note = 0;
  metadata->high_note = MIDI_MAX_NOTE;
  metadata->low_velocity = 0;
  metadata->high_velocity = MIDI_MAX_VELOCITY;
  metadata->gain = 0;
  metadata->sustain_loop_type = -1;
  metadata->sustain_begin = 0;
  metadata->sustain_end = 0;
  metadata->release_loop_type = -1;
  metadata->release_begin = 0;
  metadata->release_end = 0;

  if(common_chunk != 0)
    {
      metadata->channel_count = READ_BE_S16(
        common_chunk, COMM_CHANNEL_COUNT_OFFSET);
      metadata->frame_count = READ_BE_U32(
        common_chunk, COMM_FRAME_COUNT_OFFSET);
      metadata->bits_per_sample = READ_BE_S16(
        common_chunk, COMM_BITS_PER_SAMPLE_OFFSET);
      extended_rate[AIFF_RATE_WORD_HIGH] =
        ((int *)common_chunk)[COMM_RATE_WORD_HIGH];
      extended_rate[AIFF_RATE_WORD_MIDDLE] =
        ((int *)common_chunk)[COMM_RATE_WORD_MIDDLE];
      extended_rate[AIFF_RATE_WORD_LOW] =
        ((int *)common_chunk)[COMM_RATE_WORD_LOW];
      metadata->sample_rate = decode_ieee_extended_sample_rate(
        (unsigned char *)extended_rate);

      if(form_type == AIFC_FORM_TYPE)
        {
          metadata->compression_type = READ_BE_U32(
            common_chunk, COMM_COMPRESSION_TYPE_OFFSET);
        }
    }

  if(instrument_chunk == 0)
    {
      return metadata;
    }

  metadata->base_note = instrument_chunk[INST_BASE_NOTE_OFFSET];
  metadata->detune = instrument_chunk[INST_DETUNE_OFFSET];
  metadata->low_note = instrument_chunk[INST_LOW_NOTE_OFFSET];
  metadata->high_note = instrument_chunk[INST_HIGH_NOTE_OFFSET];
  metadata->low_velocity = instrument_chunk[INST_LOW_VELOCITY_OFFSET];
  metadata->high_velocity = instrument_chunk[INST_HIGH_VELOCITY_OFFSET];
  metadata->gain = READ_BE_S16(instrument_chunk, INST_GAIN_OFFSET);

  if(marker_chunk == 0)
    {
      return metadata;
    }

  loop_type = READ_BE_S16(instrument_chunk, INST_SUSTAIN_TYPE_OFFSET);
  if(loop_type > 0)
    {
      loop_begin = find_aiff_marker_position_unaligned(
        marker_chunk,
        READ_BE_S16(instrument_chunk, INST_SUSTAIN_BEGIN_OFFSET));
      loop_end = find_aiff_marker_position_unaligned(
        marker_chunk,
        READ_BE_S16(instrument_chunk, INST_SUSTAIN_END_OFFSET));
      metadata->sustain_loop_type = loop_type;
      metadata->sustain_begin = loop_begin;
      metadata->sustain_end = loop_end;
    }

  loop_type = READ_BE_S16(instrument_chunk, INST_RELEASE_TYPE_OFFSET);
  if(loop_type > 0)
    {
      loop_begin = find_aiff_marker_position_unaligned(
        marker_chunk,
        READ_BE_S16(instrument_chunk, INST_RELEASE_BEGIN_OFFSET));
      loop_end = find_aiff_marker_position_unaligned(
        marker_chunk,
        READ_BE_S16(instrument_chunk, INST_RELEASE_END_OFFSET));
      metadata->release_loop_type = loop_type;
      metadata->release_begin = loop_begin;
      metadata->release_end = loop_end;
    }

  return metadata;
}


#undef AIFC_FORM_TYPE
#undef AIFF_EXTENDED_RATE_WORDS
#undef AIFF_RATE_WORD_HIGH
#undef AIFF_RATE_WORD_MIDDLE
#undef AIFF_RATE_WORD_LOW
#undef COMM_CHANNEL_COUNT_OFFSET
#undef COMM_FRAME_COUNT_OFFSET
#undef COMM_BITS_PER_SAMPLE_OFFSET
#undef COMM_RATE_WORD_HIGH
#undef COMM_RATE_WORD_MIDDLE
#undef COMM_RATE_WORD_LOW
#undef COMM_COMPRESSION_TYPE_OFFSET
#undef INST_BASE_NOTE_OFFSET
#undef INST_DETUNE_OFFSET
#undef INST_LOW_NOTE_OFFSET
#undef INST_HIGH_NOTE_OFFSET
#undef INST_LOW_VELOCITY_OFFSET
#undef INST_HIGH_VELOCITY_OFFSET
#undef INST_GAIN_OFFSET
#undef INST_SUSTAIN_TYPE_OFFSET
#undef INST_SUSTAIN_BEGIN_OFFSET
#undef INST_SUSTAIN_END_OFFSET
#undef INST_RELEASE_TYPE_OFFSET
#undef INST_RELEASE_BEGIN_OFFSET
#undef INST_RELEASE_END_OFFSET
#undef MIDI_DEFAULT_BASE_NOTE
#undef MIDI_MAX_NOTE
#undef MIDI_MAX_VELOCITY
#undef READ_BE_S16
#undef READ_BE_U32
