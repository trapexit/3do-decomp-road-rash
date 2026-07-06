#include "hardware.h"

#define CEL_PIXC_UPPER_HALF_MASK 0xFFFF0000U

enum CelShadingPixcLevel
{
  CEL_SHADING_PIXC_LEVEL_ZERO = 0,
  CEL_SHADING_PIXC_LEVEL_ONE = 1,
  CEL_SHADING_PIXC_LEVEL_TWO = 2,
  CEL_SHADING_PIXC_LEVEL_THREE = 3,
  CEL_SHADING_PIXC_LEVEL_FOUR = 4,
  CEL_SHADING_PIXC_LEVEL_FIVE = 5,
  CEL_SHADING_PIXC_LEVEL_SIX = 6,
  CEL_SHADING_PIXC_LEVEL_SEVEN = 7,
  CEL_SHADING_PIXC_LEVEL_EIGHT = 8,
  CEL_SHADING_PIXC_LEVEL_NINE = 9,
  CEL_SHADING_PIXC_LEVEL_TEN = 10,
  CEL_SHADING_PIXC_LEVEL_ELEVEN = 11,
  CEL_SHADING_PIXC_LEVEL_TWELVE = 12,
  CEL_SHADING_PIXC_LEVEL_THIRTEEN = 13,
  CEL_SHADING_PIXC_LEVEL_FOURTEEN = 14,
  CEL_SHADING_PIXC_LEVEL_DEFAULT = 15
};

enum CelShadingPixcScale
{
  CEL_SHADING_PIXC_COLUMN_MIN = 0,
  CEL_SHADING_PIXC_COLUMN_MAX = 31,
  CEL_SHADING_PIXC_SCALE_THREE = 3,
  CEL_SHADING_PIXC_SCALE_FIVE = 5,
  CEL_SHADING_PIXC_SCALE_SEVEN = 7,
  CEL_SHADING_PIXC_SCALE_NINE = 9,
  CEL_SHADING_PIXC_SCALE_ELEVEN = 11,
  CEL_SHADING_PIXC_SCALE_THIRTEEN = 13,
  CEL_SHADING_PIXC_SCALE_FIFTEEN = 15,
  CEL_SHADING_PIXC_DIVIDE_BY_TWO_SHIFT = 1,
  CEL_SHADING_PIXC_DIVIDE_BY_FOUR_SHIFT = 2,
  CEL_SHADING_PIXC_DIVIDE_BY_EIGHT_SHIFT = 3,
  CEL_SHADING_PIXC_DIVIDE_BY_SIXTEEN_SHIFT = 4
};

static
void
write_cel_pixc_low_half(unsigned int *pixc_entry,
                        unsigned int  low_half)
{
  *pixc_entry = (*pixc_entry & CEL_PIXC_UPPER_HALF_MASK) | low_half;
}


void
write_cel_shading_pixc_entry(int           shade_level,
                             int           shade_column,
                             unsigned int *pixc_entry)
{
  unsigned int pixc;

  if(shade_column < CEL_SHADING_PIXC_COLUMN_MIN)
    {
      shade_column = CEL_SHADING_PIXC_COLUMN_MIN;
    }
  else if(shade_column > CEL_SHADING_PIXC_COLUMN_MAX)
    {
      shade_column = CEL_SHADING_PIXC_COLUMN_MAX;
    }

  if(shade_level < CEL_SHADING_PIXC_LEVEL_ZERO)
    {
      shade_level = CEL_SHADING_PIXC_LEVEL_ZERO;
    }

  switch(shade_level)
    {
    default:
    case CEL_SHADING_PIXC_LEVEL_DEFAULT:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_FIFTEEN) >>
              CEL_SHADING_PIXC_DIVIDE_BY_SIXTEEN_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_1 | PPMPC_SF_16 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_ZERO:
      pixc = PPMPC_MF_8 | PPMPC_SF_8;
      break;
    case CEL_SHADING_PIXC_LEVEL_ONE:
    case CEL_SHADING_PIXC_LEVEL_TWO:
      pixc = (unsigned int)
             (shade_column >> CEL_SHADING_PIXC_DIVIDE_BY_FOUR_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_7 | PPMPC_SF_4 | PPMPC_2S_CCB | PPMPC_2D_2;
      break;
    case CEL_SHADING_PIXC_LEVEL_THREE:
    case CEL_SHADING_PIXC_LEVEL_FOUR:
      pixc = (unsigned int)
             (shade_column >> CEL_SHADING_PIXC_DIVIDE_BY_TWO_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_6 | PPMPC_SF_4 | PPMPC_2S_CCB | PPMPC_2D_2;
      break;
    case CEL_SHADING_PIXC_LEVEL_FIVE:
    case CEL_SHADING_PIXC_LEVEL_SIX:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_THREE) >>
              CEL_SHADING_PIXC_DIVIDE_BY_EIGHT_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_5 | PPMPC_SF_8 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_SEVEN:
    case CEL_SHADING_PIXC_LEVEL_EIGHT:
      pixc = (unsigned int)shade_column << PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_8 | PPMPC_SF_8 | PPMPC_2S_CCB | PPMPC_2D_2;
      break;
    case CEL_SHADING_PIXC_LEVEL_NINE:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_NINE) >>
              CEL_SHADING_PIXC_DIVIDE_BY_SIXTEEN_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_7 | PPMPC_SF_16 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_TEN:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_FIVE) >>
              CEL_SHADING_PIXC_DIVIDE_BY_EIGHT_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_3 | PPMPC_SF_8 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_ELEVEN:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_ELEVEN) >>
              CEL_SHADING_PIXC_DIVIDE_BY_SIXTEEN_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_5 | PPMPC_SF_16 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_TWELVE:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_THREE) >>
              CEL_SHADING_PIXC_DIVIDE_BY_FOUR_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_2 | PPMPC_SF_8 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_THIRTEEN:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_THIRTEEN) >>
              CEL_SHADING_PIXC_DIVIDE_BY_SIXTEEN_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_3 | PPMPC_SF_16 | PPMPC_2S_CCB;
      break;
    case CEL_SHADING_PIXC_LEVEL_FOURTEEN:
      pixc = (unsigned int)
             ((shade_column * CEL_SHADING_PIXC_SCALE_SEVEN) >>
              CEL_SHADING_PIXC_DIVIDE_BY_EIGHT_SHIFT) <<
             PPMPC_AV_SHIFT;
      pixc |= PPMPC_MF_1 | PPMPC_SF_8 | PPMPC_2S_CCB;
      break;
    }

  write_cel_pixc_low_half(pixc_entry, pixc);
}
