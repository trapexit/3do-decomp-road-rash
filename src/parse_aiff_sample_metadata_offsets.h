#pragma once

/* Raw layout offsets extracted verbatim from parse_aiff_sample_metadata.c.
 * This header has a single consumer; the values are not proven against any
 * structure and remain offset-oriented until aggregate evidence exists. */

#define COMM_CHANNEL_COUNT_OFFSET 0x08
#define COMM_FRAME_COUNT_OFFSET 0x0A
#define COMM_BITS_PER_SAMPLE_OFFSET 0x0E
#define COMM_COMPRESSION_TYPE_OFFSET 0x1A

#define INST_BASE_NOTE_OFFSET 0x08
#define INST_DETUNE_OFFSET 0x09
#define INST_LOW_NOTE_OFFSET 0x0A
#define INST_HIGH_NOTE_OFFSET 0x0B
#define INST_LOW_VELOCITY_OFFSET 0x0C
#define INST_HIGH_VELOCITY_OFFSET 0x0D
#define INST_GAIN_OFFSET 0x0E
#define INST_SUSTAIN_TYPE_OFFSET 0x10
#define INST_SUSTAIN_BEGIN_OFFSET 0x12
#define INST_SUSTAIN_END_OFFSET 0x14
#define INST_RELEASE_TYPE_OFFSET 0x16
#define INST_RELEASE_BEGIN_OFFSET 0x18
#define INST_RELEASE_END_OFFSET 0x1A
