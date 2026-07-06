#ifndef AIFF_SAMPLE_METADATA_H
#define AIFF_SAMPLE_METADATA_H

typedef struct AiffSampleMetadata
{
  int channel_count;
  int frame_count;
  int bits_per_sample;
  int sample_rate;
  int compression_type;
  signed char base_note;
  signed char detune;
  signed char low_note;
  signed char high_note;
  signed char low_velocity;
  signed char high_velocity;
  int gain;
  int sustain_loop_type;
  int sustain_begin;
  int sustain_end;
  int release_loop_type;
  int release_begin;
  int release_end;
} AiffSampleMetadata;

#endif
