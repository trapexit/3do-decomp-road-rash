#include "display_cel_runtime.h"
#include "road_geometry.h"

enum RoadCelDimensionFactorConstants
{
  ROAD_CEL_POSITIVE_DIMENSION_FACTOR_SHIFT = 15
};

int *
map_road_bounds_to_cel(CCB                 *cel,
                       const RoadCelBounds *bounds)
{
  int coordinate_delta;
  int dimension_factor;

  if(cel == 0)
    {
      return 0;
    }

  if((cel->ccb_Width & cel->ccb_Height) < 0)
    {
      return map_fixed_road_bounds_to_cel(cel, bounds);
    }

  cel->ccb_XPos = bounds->left;
  cel->ccb_YPos = bounds->top;
  cel->ccb_HDY = 0;
  cel->ccb_VDX = 0;
  cel->ccb_HDDX = 0;
  cel->ccb_HDDY = 0;

  coordinate_delta =
    (bounds->right - bounds->left) >> ROAD_8_8_FRACTION_BITS;
  dimension_factor = cel->ccb_Width;
  if(dimension_factor >= 0)
    {
      dimension_factor >>= ROAD_CEL_POSITIVE_DIMENSION_FACTOR_SHIFT;
    }
  else
    {
      dimension_factor =
        (int)((unsigned int)ROAD_FIXED_ONE >>
              (dimension_factor & ROAD_SHIFT_COUNT_MASK));
    }
  cel->ccb_HDX =
    (coordinate_delta * dimension_factor) >>
    DISPLAY_CEL_MAPPING_SCALE_SHIFT;

  coordinate_delta =
    (bounds->bottom - bounds->top) >> ROAD_8_8_FRACTION_BITS;
  dimension_factor = cel->ccb_Height;
  if(dimension_factor >= 0)
    {
      dimension_factor >>= ROAD_CEL_POSITIVE_DIMENSION_FACTOR_SHIFT;
    }
  else
    {
      dimension_factor =
        (int)((unsigned int)ROAD_FIXED_ONE >>
              (dimension_factor & ROAD_SHIFT_COUNT_MASK));
    }
  cel->ccb_VDY =
    (coordinate_delta * dimension_factor) >>
    ROAD_8_8_FRACTION_BITS;
  return (int *)&cel->ccb_VDY;
}
