#pragma once

/* Raw layout offsets extracted verbatim from decode_ieee_extended_sample_rate.c.
 * This header has a single consumer; the values are not proven against any
 * structure and remain offset-oriented until aggregate evidence exists. */

#define AIFF_MARK_FIRST_ENTRY_OFFSET 0x0A
#define AIFF_MARK_ID_OFFSET 0
#define AIFF_MARK_POSITION_HIGH_OFFSET 2
#define AIFF_MARK_POSITION_LOW_OFFSET 4
#define AIFF_MARK_COUNT_OFFSET 8
#define AIFF_CHUNK_SIZE_OFFSET 4
#define IEEE_EXTENDED_MANTISSA_HIGH_OFFSET 2
#define IEEE_EXTENDED_MANTISSA_LOW_OFFSET 6
