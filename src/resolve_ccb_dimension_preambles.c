#include "bss_early_data.h"
#include "front_end_cel_runtime.h"

/* Reconstructed from the original function at 0x0001CAA8. */

void
resolve_ccb_dimension_preambles(CCB *ccb)
{
  unsigned int table_index;
  int encoded_dimension;

  if(ccb == 0)
    {
      return;
    }

  encoded_dimension = ccb->ccb_Width;
  if(encoded_dimension >= 0)
    {
      table_index = (unsigned int)encoded_dimension &
                    CANS_CCB_DIMENSION_LITERAL_MASK;
    }
  else
    {
      table_index = 1U <<
                    ((unsigned int)encoded_dimension &
                     CANS_CCB_DIMENSION_EXPONENT_MASK);
    }
  ccb->ccb_Width = gCelDimensionPreambleTable[table_index];

  encoded_dimension = ccb->ccb_Height;
  if(encoded_dimension >= 0)
    {
      table_index = (unsigned int)encoded_dimension &
                    CANS_CCB_DIMENSION_LITERAL_MASK;
    }
  else
    {
      table_index = 1U <<
                    ((unsigned int)encoded_dimension &
                     CANS_CCB_DIMENSION_EXPONENT_MASK);
    }
  ccb->ccb_Height = gCelDimensionPreambleTable[table_index];
}
