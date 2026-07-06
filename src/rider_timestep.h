#pragma once

#include "division_helpers.h"

// These positive steps dominate the captured rider updates. Keep the legacy
// divider for every other value, including zero and negative elapsed ticks.
// Vertical motion keeps its distinct historical 1-/16-tick paths outside
// this helper; their overflow/rounding behavior is not the general divider's.
static
__inline
int
_rider_timestep_quotient(int timestep_, int numerator_)
{
  SignedDivisionResult result;

  switch(timestep_)
    {
    case 1:
      // The legacy divider caps the INT_MIN magnitude quotient at INT_MAX.
      return (numerator_ + ((unsigned int)numerator_ == DIVISION_SIGN_BIT_MASK));
    case (1 << DIVISION_BIT_1):
      if(numerator_ < 0)
        numerator_ += ((1 << DIVISION_BIT_1) - 1);
      return (numerator_ >> DIVISION_BIT_1);
    case (1 << DIVISION_BIT_2):
      if(numerator_ < 0)
        numerator_ += ((1 << DIVISION_BIT_2) - 1);
      return (numerator_ >> DIVISION_BIT_2);
    case (1 << DIVISION_BIT_3):
      if(numerator_ < 0)
        numerator_ += ((1 << DIVISION_BIT_3) - 1);
      return (numerator_ >> DIVISION_BIT_3);
    default:
      break;
    }

  result = signed_divide_with_remainder((unsigned int)timestep_,
                                        (unsigned int)numerator_);
  return result.quotient;
}
