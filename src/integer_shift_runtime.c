#include "integer_shift_runtime.h"

#define INTEGER_SHIFT_SIGN_BIT 0x80000000U

typedef char IntegerShiftRequires32BitUnsigned[
    (sizeof(unsigned int) == 4) ? 1 : -1];


static
int
signed_32_from_bits(unsigned int bits)
{
  if(bits < INTEGER_SHIFT_SIGN_BIT)
    {
      return (int)bits;
    }
  return -1 - (int)(~bits);
}


int
shift_left_wrapping_32(int          value,
                       unsigned int shift_count)
{
  return signed_32_from_bits(
    (unsigned int)value << shift_count);
}


int
add_wrapping_32(int left,
                int right)
{
  return signed_32_from_bits(
    (unsigned int)left + (unsigned int)right);
}


int
negate_wrapping_32(int value)
{
  return signed_32_from_bits(0U - (unsigned int)value);
}

#undef INTEGER_SHIFT_SIGN_BIT
