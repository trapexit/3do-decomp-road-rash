#include "division_helpers.h"

__value_in_regs SignedDivisionResult
signed_divide_by_ten_with_remainder(int value)
{
  SignedDivisionResult result;
  unsigned int magnitude;
  unsigned int quotient;
  unsigned int remainder;

  magnitude = (unsigned int)value;
  if(value < 0)
    {
      magnitude = 0U - magnitude;
    }

  quotient = magnitude >> 1;
  quotient += quotient >> 1;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_4;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_8;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_16;
  quotient >>= DIVIDE_BY_TEN_NORMALIZE_SHIFT;

  remainder = magnitude -
              ((quotient +
                (quotient << DIVIDE_BY_TEN_QUINTUPLE_SHIFT)) << 1);
  if(remainder >= DIVIDE_BY_TEN_DIVISOR)
    {
      quotient++;
      remainder -= DIVIDE_BY_TEN_DIVISOR;
    }

  if(value < 0)
    {
      result.quotient = -(int)quotient;
      result.remainder = -(int)remainder;
    }
  else
    {
      result.quotient = (int)quotient;
      result.remainder = (int)remainder;
    }
  return result;
}
