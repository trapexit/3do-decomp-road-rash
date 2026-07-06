#include "division_helpers.h"

#define UNSIGNED_DIVIDE_STEP(shift_) \
  do \
    { \
      if(divisor_ <= (remainder >> (shift_))) \
        { \
          remainder = (remainder - (divisor_ << (shift_))); \
          quotient = (quotient | (1U << (shift_))); \
        } \
    } \
  while(0)

__value_in_regs
UnsignedDivisionResult
unsigned_divide_with_remainder(unsigned int divisor_,
                               unsigned int numerator_)
{
  UnsignedDivisionResult result;
  unsigned int quotient = 0U;
  unsigned int remainder = numerator_;

  if(divisor_ <= 1U)
    {
      // A zero divisor historically sets every quotient bit without subtracting.
      result.quotient = (divisor_ == 0U) ? ~0U : numerator_;
      result.remainder = (divisor_ == 0U) ? numerator_ : 0U;
      return result;
    }

  if(divisor_ <= remainder)
    {
      // Each block is independent: higher quotient bits are already in place.
      if(divisor_ <= (remainder >> DIVISION_BIT_16))
        {
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_31);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_30);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_29);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_28);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_27);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_26);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_25);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_24);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_23);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_22);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_21);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_20);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_19);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_18);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_17);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_16);
        }

      if(divisor_ <= (remainder >> DIVISION_BIT_8))
        {
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_15);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_14);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_13);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_12);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_11);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_10);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_9);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_8);
        }

      if(divisor_ <= (remainder >> DIVISION_BIT_4))
        {
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_7);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_6);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_5);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_4);
        }

      if(divisor_ <= (remainder >> DIVISION_BIT_2))
        {
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_3);
          UNSIGNED_DIVIDE_STEP(DIVISION_BIT_2);
        }

      UNSIGNED_DIVIDE_STEP(DIVISION_BIT_1);
      UNSIGNED_DIVIDE_STEP(DIVISION_BIT_0);
    }

  result.quotient = quotient;
  result.remainder = remainder;
  return result;
}

#undef UNSIGNED_DIVIDE_STEP
