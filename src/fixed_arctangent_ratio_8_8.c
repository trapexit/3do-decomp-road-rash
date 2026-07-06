#include "rw_semantic_data.h"

enum FixedArctangentRatioConstant
{
  FIXED_ARCTANGENT_RATIO_ONE = 0x100,
  FIXED_ARCTANGENT_RECIPROCAL_NUMERATOR = 0x10000,
  FIXED_ARCTANGENT_QUARTER_TURN = 0x100
};

typedef struct FixedArctangentDivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} FixedArctangentDivModResult;

__value_in_regs FixedArctangentDivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);

int
fixed_arctangent_ratio_8_8(int ratio)
{
  FixedArctangentDivModResult division;
  const unsigned char *lookup;

  if(ratio < 0)
    {
      return -fixed_arctangent_ratio_8_8(-ratio);
    }
  if(ratio <= FIXED_ARCTANGENT_RATIO_ONE)
    {
      lookup = &display_runtime_globals.easing_lookup[0];
      return lookup[ratio];
    }
  if(ratio >= FIXED_ARCTANGENT_RECIPROCAL_NUMERATOR)
    {
      return FIXED_ARCTANGENT_QUARTER_TURN;
    }

  division = signed_divide_with_remainder(
    ratio, FIXED_ARCTANGENT_RECIPROCAL_NUMERATOR);
  return FIXED_ARCTANGENT_QUARTER_TURN -
         fixed_arctangent_ratio_8_8((int)division.quotient);
}
