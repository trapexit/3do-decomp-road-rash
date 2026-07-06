/* Reconstructed from the original function at 0x00000494. */

#include "division_helpers.h"

__value_in_regs UnsignedDivisionResult
unsigned_divide_by_ten_with_remainder(unsigned int value)
{
  UnsignedDivisionResult result;
  unsigned int quotient;
  unsigned int remainder;

  quotient = value >> DIVISION_BIT_1;
  quotient += quotient >> DIVISION_BIT_1;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_4;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_8;
  quotient += quotient >> DIVIDE_BY_TEN_REFINE_SHIFT_16;
  quotient >>= DIVIDE_BY_TEN_NORMALIZE_SHIFT;

  remainder = value -
              ((quotient +
                (quotient << DIVIDE_BY_TEN_QUINTUPLE_SHIFT)) <<
               DIVISION_BIT_1);
  if(remainder >= DIVIDE_BY_TEN_DIVISOR)
    {
      quotient += 1U;
      remainder -= DIVIDE_BY_TEN_DIVISOR;
    }

  result.quotient = quotient;
  result.remainder = remainder;
  return result;
}
