#include "decode_ieee_extended_sample_rate_offsets.h"

#define AIFF_MARK_CHUNK_ID 0x4D41524B
#define IEEE_EXTENDED_EXPONENT_MASK 0x7F
#define IEEE_EXTENDED_SIGN_MASK 0x80
#define IEEE_EXTENDED_INFINITY_EXPONENT 0x7FFF
#define IEEE_EXTENDED_INTEGER_BIAS 0x400E
#define AIFF_MARK_FIXED_ENTRY_BYTES 6
#define AIFF_MARK_POSITION_SHIFT 16
#define AIFF_MARK_INDEX_MASK 0xFFFF
#define AIFF_PSTRING_LENGTH_BYTES 1
#define AIFF_ALIGNMENT_BYTES 2
#define BIG_ENDIAN_BYTE_SHIFT 8
#define BIG_ENDIAN_HALFWORD_SHIFT 16
#define BIG_ENDIAN_HIGH_BYTE_SHIFT 24
#define BYTE_INDEX_TWO 2
#define BYTE_INDEX_THREE 3
#define IEEE_EXTENDED_WORD_BITS 32

static
int
read_big_endian_u16(unsigned char *bytes)
{
  return (bytes[0] << BIG_ENDIAN_BYTE_SHIFT) | bytes[1];
}


static
unsigned int
read_big_endian_u32(unsigned char *bytes)
{
  return ((unsigned int)bytes[0] << BIG_ENDIAN_HIGH_BYTE_SHIFT) |
         ((unsigned int)bytes[1] << BIG_ENDIAN_HALFWORD_SHIFT) |
         ((unsigned int)bytes[BYTE_INDEX_TWO] <<
          BIG_ENDIAN_BYTE_SHIFT) |
         (unsigned int)bytes[BYTE_INDEX_THREE];
}


static
unsigned int
shift_right_like_arm_lsr(unsigned int value,
                         int          shift)
{
  if(shift >= IEEE_EXTENDED_WORD_BITS)
    {
      return 0;
    }
  return value >> shift;
}


int
decode_ieee_extended_sample_rate(unsigned char *encoded_rate)
{
  int exponent;
  unsigned int mantissa_high;
  unsigned int mantissa_low;
  int shift;
  int value;

  exponent = ((encoded_rate[0] & IEEE_EXTENDED_EXPONENT_MASK) <<
              BIG_ENDIAN_BYTE_SHIFT) |
             encoded_rate[1];
  mantissa_high = read_big_endian_u32(
    encoded_rate + IEEE_EXTENDED_MANTISSA_HIGH_OFFSET);
  mantissa_low = read_big_endian_u32(
    encoded_rate + IEEE_EXTENDED_MANTISSA_LOW_OFFSET);

  if(exponent == 0)
    {
      if(mantissa_high == 0 && mantissa_low == 0)
        {
          value = 0;
        }
      else
        {
          shift = exponent - IEEE_EXTENDED_INTEGER_BIAS;
          if(shift > 0)
            {
              value = -1;
            }
          else if(shift >= 0)
            {
              value = (int)mantissa_high;
            }
          else
            {
              value = (int)shift_right_like_arm_lsr(
                mantissa_high, -shift);
            }
        }
    }
  else if(exponent == IEEE_EXTENDED_INFINITY_EXPONENT)
    {
      value = -1;
    }
  else
    {
      shift = exponent - IEEE_EXTENDED_INTEGER_BIAS;
      if(shift > 0)
        {
          value = -1;
        }
      else if(shift >= 0)
        {
          value = (int)mantissa_high;
        }
      else
        {
          value = (int)shift_right_like_arm_lsr(
            mantissa_high, -shift);
        }
    }

  if((encoded_rate[0] & IEEE_EXTENDED_SIGN_MASK) != 0)
    {
      return -value;
    }
  return value;
}


int
find_aiff_marker_position(unsigned char *marker_chunk,
                          int            marker_id)
{
  int target_id;
  int position;
  int index;
  int count;
  unsigned char *cursor;

  target_id = (int)(short)marker_id;
  position = 0;
  index = 0;

  if(*(int *)marker_chunk != AIFF_MARK_CHUNK_ID)
    {
      return 0;
    }

  if(*(int *)(marker_chunk + AIFF_CHUNK_SIZE_OFFSET) <= 0)
    {
      return 0;
    }

  count = (marker_chunk[AIFF_MARK_COUNT_OFFSET] <<
           BIG_ENDIAN_BYTE_SHIFT) |
          marker_chunk[AIFF_MARK_COUNT_OFFSET + 1];
  cursor = marker_chunk + AIFF_MARK_FIRST_ENTRY_OFFSET;
  while(index < count)
    {
      if(target_id == (int)(short)(
           (cursor[AIFF_MARK_ID_OFFSET] << BIG_ENDIAN_BYTE_SHIFT) |
           cursor[AIFF_MARK_ID_OFFSET + 1]))
        {
          position = (((cursor[AIFF_MARK_POSITION_HIGH_OFFSET] <<
                        BIG_ENDIAN_BYTE_SHIFT) |
                       cursor[AIFF_MARK_POSITION_HIGH_OFFSET + 1]) <<
                      AIFF_MARK_POSITION_SHIFT) |
                     (cursor[AIFF_MARK_POSITION_LOW_OFFSET] <<
                      BIG_ENDIAN_BYTE_SHIFT) |
                     cursor[AIFF_MARK_POSITION_LOW_OFFSET + 1];
          break;
        }

      cursor += AIFF_MARK_FIXED_ENTRY_BYTES;
      cursor += *cursor + AIFF_PSTRING_LENGTH_BYTES;
      if(((int)cursor & (AIFF_ALIGNMENT_BYTES - 1)) != 0)
        {
          cursor++;
        }
      index = (index + 1) & AIFF_MARK_INDEX_MASK;
    }

  return position;
}


#undef AIFF_MARK_CHUNK_ID
#undef IEEE_EXTENDED_EXPONENT_MASK
#undef IEEE_EXTENDED_SIGN_MASK
#undef IEEE_EXTENDED_INFINITY_EXPONENT
#undef IEEE_EXTENDED_INTEGER_BIAS
#undef AIFF_MARK_FIRST_ENTRY_OFFSET
#undef AIFF_MARK_FIXED_ENTRY_BYTES
#undef AIFF_MARK_ID_OFFSET
#undef AIFF_MARK_POSITION_HIGH_OFFSET
#undef AIFF_MARK_POSITION_LOW_OFFSET
#undef AIFF_MARK_POSITION_SHIFT
#undef AIFF_MARK_INDEX_MASK
#undef AIFF_MARK_COUNT_OFFSET
#undef AIFF_CHUNK_SIZE_OFFSET
#undef AIFF_PSTRING_LENGTH_BYTES
#undef AIFF_ALIGNMENT_BYTES
#undef BIG_ENDIAN_BYTE_SHIFT
#undef BIG_ENDIAN_HALFWORD_SHIFT
#undef BIG_ENDIAN_HIGH_BYTE_SHIFT
#undef BYTE_INDEX_TWO
#undef BYTE_INDEX_THREE
#undef IEEE_EXTENDED_MANTISSA_HIGH_OFFSET
#undef IEEE_EXTENDED_MANTISSA_LOW_OFFSET
#undef IEEE_EXTENDED_WORD_BITS
