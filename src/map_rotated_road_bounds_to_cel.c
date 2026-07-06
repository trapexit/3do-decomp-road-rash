#include "division_helpers.h"
#include "road_geometry.h"
#include "road_projected_cel.h"

enum RotatedRoadCelConstant
{
  ROTATED_CEL_LITERAL_DIMENSION_SHIFT = 23,
  ROTATED_CEL_EXPONENT_MASK = 0x1F,
  ROTATED_CEL_ANGLE_SIGN_BIT = 0x800000,
  ROTATED_CEL_WORLD_FRACTION_BITS = 16,
  ROTATED_CEL_SCALE_FRACTION_BITS = 8,
  ROTATED_CEL_MATRIX_SCALE_SHIFT = 4
};

#define ROTATED_CEL_ANGLE_SIGN_EXTENSION ((int)0xFF000000U)

int
fixed_cosine(int angle);
int
fixed_sine(int angle);

static
int
decode_rotated_cel_dimension(int encoded_dimension)
{
  if(encoded_dimension >= 0)
    {
      return (int)(((unsigned int)encoded_dimension <<
                    ROTATED_CEL_LITERAL_DIMENSION_SHIFT) >>
                   ROTATED_CEL_LITERAL_DIMENSION_SHIFT);
    }
  return 1 << (encoded_dimension & ROTATED_CEL_EXPONENT_MASK);
}


void
map_rotated_road_bounds_to_cel(CCB                     *cel,
                               const RoadCelBounds     *bounds,
                               RoadProjectedCelExtents *extents,
                               int                      angle)
{
  SignedDivisionResult division;
  int cosine;
  int sine;
  int horizontal_factor;
  int vertical_factor;
  int horizontal_delta;
  int vertical_delta;
  int base;
  int inverted;
  int horizontal_dy;
  int vertical_dx;
  int vertical_dy;

  if(cel == 0)
    {
      return;
    }

  if(angle >= ROTATED_CEL_ANGLE_SIGN_BIT)
    {
      angle += ROTATED_CEL_ANGLE_SIGN_EXTENSION;
    }

  cosine = fixed_cosine(angle);
  sine = fixed_sine(angle);

  division = signed_divide_with_remainder(
    (unsigned int)decode_rotated_cel_dimension(cel->ccb_Width),
    (unsigned int)(((bounds->right - bounds->left) >>
                    ROTATED_CEL_WORLD_FRACTION_BITS) <<
                   ROTATED_CEL_SCALE_FRACTION_BITS));
  horizontal_factor = division.quotient;
  division = signed_divide_with_remainder(
    (unsigned int)decode_rotated_cel_dimension(cel->ccb_Height),
    (unsigned int)(((bounds->bottom - bounds->top) >>
                    ROTATED_CEL_WORLD_FRACTION_BITS) <<
                   ROTATED_CEL_SCALE_FRACTION_BITS));
  vertical_factor = division.quotient;

  horizontal_delta =
    (horizontal_factor * extents->horizontal) >>
    ROTATED_CEL_SCALE_FRACTION_BITS;
  extents->horizontal = horizontal_delta;
  vertical_delta =
    (vertical_factor * extents->vertical) >>
    ROTATED_CEL_SCALE_FRACTION_BITS;
  extents->vertical = vertical_delta;

  inverted = 0;
  if(bounds->left > bounds->right)
    {
      inverted = 1;
      horizontal_delta = -horizontal_delta -
                         (bounds->left - bounds->right);
      extents->horizontal = horizontal_delta;
    }

  base = bounds->left + horizontal_delta;
  cel->ccb_XPos =
    base - cosine *
    (horizontal_delta >> ROTATED_CEL_WORLD_FRACTION_BITS) +
    sine * (vertical_delta >> ROTATED_CEL_WORLD_FRACTION_BITS);

  base = bounds->top + vertical_delta;
  cel->ccb_YPos =
    base - sine *
    (horizontal_delta >> ROTATED_CEL_WORLD_FRACTION_BITS) -
    cosine * (vertical_delta >> ROTATED_CEL_WORLD_FRACTION_BITS);

  cel->ccb_HDX =
    ((horizontal_factor * cosine) >>
     ROTATED_CEL_SCALE_FRACTION_BITS) <<
      ROTATED_CEL_MATRIX_SCALE_SHIFT;
  horizontal_dy =
    ((vertical_factor * sine) >>
     ROTATED_CEL_SCALE_FRACTION_BITS) <<
      ROTATED_CEL_MATRIX_SCALE_SHIFT;
  vertical_dx = -((horizontal_factor * sine) >>
                  ROTATED_CEL_SCALE_FRACTION_BITS);
  vertical_dy = (vertical_factor * cosine) >>
                ROTATED_CEL_SCALE_FRACTION_BITS;

  cel->ccb_VDY = vertical_dy;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;

  if(inverted != 0)
    {
      horizontal_dy = -horizontal_dy;
      vertical_dx = -vertical_dx;
    }

  cel->ccb_HDY = horizontal_dy;
  cel->ccb_VDX = vertical_dx;
}


#undef ROTATED_CEL_ANGLE_SIGN_EXTENSION
