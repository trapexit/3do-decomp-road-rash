/* Reconstructed from the original function at 0x0004E384. */



#include "operamath.h"
#include "rw_semantic_data.h"

enum
{
  FIXED_RATIO_FRACTION_BITS = 8,
  FIXED_RATIO_FRACTION_MASK = 0xFF,
  FIXED_RATIO_INTERVAL = 0x100,
  FIXED_ANGLE_QUARTER_TURN = 0x200000,
  FIXED_ANGLE_QUADRANT_SHIFT = 21,
  FIXED_ANGLE_LOWER_HALF_QUADRANT = 4,
  FIXED_ANGLE_HORIZONTAL_MIRROR = 3
};

int
fixed_vector_angle_24(int horizontal,
                      int vertical)
{
  int quadrant;
  int mirror;
  int packed;
  int fraction;
  int index;
  int *table;
  int value;
  unsigned int lower;
  unsigned int upper;
  int tmp;

  if(horizontal == 0 && vertical == 0)
    {
      return 0;
    }

  quadrant = 0;
  mirror = 0;

  if(vertical < 0)
    {
      quadrant = FIXED_ANGLE_LOWER_HALF_QUADRANT;
      horizontal = -horizontal;
      vertical = -vertical;
    }

  if(horizontal < 0)
    {
      quadrant ^= FIXED_ANGLE_HORIZONTAL_MIRROR;
      mirror = 1;
      horizontal = -horizontal;
    }

  if(horizontal < vertical)
    {
      quadrant ^= 1;
      mirror ^= 1;
      tmp = horizontal;
      horizontal = vertical;
      vertical = tmp;
    }

  packed = (int)DivUF16((ufrac16)vertical, (ufrac16)horizontal);
  index = (int)((unsigned int)packed >> FIXED_RATIO_FRACTION_BITS);
  fraction = packed & FIXED_RATIO_FRACTION_MASK;

  table = (int *)operamath_atan_ratio_table;
  /* lower * (interval - fraction) + upper * fraction equals
     lower * interval + (upper - lower) * fraction.  The products wrap as
     unsigned values, matching the 32-bit wrap of the signed intermediates,
     and the following shift already reads the word as unsigned. */
  lower = (unsigned int)table[index];
  upper = (unsigned int)table[index + 1];
  value = (int)(lower * FIXED_RATIO_INTERVAL +
                (upper - lower) * (unsigned int)fraction);
  value = (int)((unsigned int)value >> FIXED_RATIO_FRACTION_BITS);

  if(mirror != 0)
    {
      value = FIXED_ANGLE_QUARTER_TURN - value;
    }

  return value + (quadrant << FIXED_ANGLE_QUADRANT_SHIFT);
}
