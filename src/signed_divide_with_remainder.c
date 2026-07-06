#include "division_helpers.h"

#define DIVIDE_MAGNITUDE_STEP(shift_) \
  do \
    { \
      if(denom <= (remainder >> (shift_))) \
        { \
          remainder = (remainder - (denom << (shift_))); \
          quotient = (quotient | (1U << (shift_))); \
        } \
    } \
  while(0)

__value_in_regs
SignedDivisionResult
signed_divide_with_remainder(unsigned int divisor_,
                             unsigned int numerator_)
{
  SignedDivisionResult result;
  unsigned int quotient = 0U;
  unsigned int denom = divisor_;
  unsigned int remainder = numerator_;
  unsigned int sign_quotient = ((divisor_ ^ numerator_) & DIVISION_SIGN_BIT_MASK);
  unsigned int sign_remainder = (numerator_ & DIVISION_SIGN_BIT_MASK);

  if(sign_remainder != 0U)
    remainder = (0U - remainder);
  if((denom & DIVISION_SIGN_BIT_MASK) != 0U)
    denom = (0U - denom);

  if(denom <= 1U)
    {
      // The legacy magnitude quotient has only 31 bits, even for INT_MIN / 1.
      quotient = (DIVISION_SIGN_BIT_MASK - 1U);
      if(denom != 0U)
        {
          quotient = (remainder - (remainder >> DIVISION_BIT_31));
          remainder = (remainder >> DIVISION_BIT_31);
        }
    }
  else if(denom <= remainder)
    {
      // Skip blocks whose quotient bits must be zero. Setting bits directly
      // avoids shifting an already accumulated quotient through those blocks.
      if(denom <= (remainder >> DIVISION_BIT_16))
        {
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_30);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_29);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_28);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_27);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_26);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_25);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_24);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_23);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_22);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_21);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_20);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_19);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_18);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_17);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_16);
        }

      if(denom <= (remainder >> DIVISION_BIT_8))
        {
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_15);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_14);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_13);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_12);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_11);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_10);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_9);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_8);
        }

      if(denom <= (remainder >> DIVISION_BIT_4))
        {
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_7);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_6);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_5);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_4);
        }

      if(denom <= (remainder >> DIVISION_BIT_2))
        {
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_3);
          DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_2);
        }

      DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_1);
      DIVIDE_MAGNITUDE_STEP(DIVISION_BIT_0);
    }

  if(sign_remainder != 0U)
    remainder = (0U - remainder);
  if(sign_quotient != 0U)
    quotient = (0U - quotient);

  result.quotient = (int)quotient;
  result.remainder = (int)remainder;
  return result;
}

#undef DIVIDE_MAGNITUDE_STEP
