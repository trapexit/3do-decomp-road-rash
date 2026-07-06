#include "bss_early_data.h"
#include "division_helpers.h"

enum CelDimensionPreambleConstant
{
  CEL_DIMENSION_FIRST = 1,
  CEL_DIMENSION_LIMIT = 0x200,
  CEL_DIMENSION_FRACTION = 0x400000,
  CEL_DIMENSION_QUOTIENT_SHIFT = 9
};

#define CEL_DIMENSION_POWER_OF_TWO_FLAG 0x80000000U

void
initialize_cel_dimension_preamble_table(void)
{
  int index;
  int shift;
  unsigned int value;
  int *slot;
  SignedDivisionResult division;

  slot = (int *)&gCelDimensionPreambleTable[CEL_DIMENSION_FIRST];
  index = CEL_DIMENSION_FIRST;
  do
    {
      if((index & -index) == index)
        {
          value = (unsigned int)index >> 1;
          shift = 0;
          if(value != 0)
            {
              do
                {
                  value >>= 1;
                  shift += 1;
                } while(value != 0);
            }
          *slot = shift | (int)CEL_DIMENSION_POWER_OF_TWO_FLAG;
        }
      else
        {
          division = signed_divide_with_remainder(
            (unsigned int)index, CEL_DIMENSION_FRACTION);
          *slot = index |
                  (division.quotient << CEL_DIMENSION_QUOTIENT_SHIFT);
        }
      slot++;
      index++;
    } while(index < CEL_DIMENSION_LIMIT);
}


#undef CEL_DIMENSION_POWER_OF_TWO_FLAG
