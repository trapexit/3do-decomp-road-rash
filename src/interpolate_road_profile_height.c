#include "division_helpers.h"
#include "road_segment_runtime.h"

enum RoadProfileInterpolationValue
{
  ROAD_PROFILE_INTERPOLATION_FRACTION_BITS = 16,
  ROAD_PROFILE_INTERPOLATION_FIXED_ONE =
    1 << ROAD_PROFILE_INTERPOLATION_FRACTION_BITS
};

int
interpolate_road_profile_height(const RoadLanePoint *start,
                                const RoadLanePoint *end,
                                int                  lateral_position)
{
  SignedDivisionResult division;
  int start_x;
  int start_y;
  int slope;

  start_x = start->x;
  start_y = start->y;
  division = signed_divide_with_remainder(
    (unsigned int)(end->x - start_x),
    (unsigned int)((end->y - start_y) *
                   ROAD_PROFILE_INTERPOLATION_FIXED_ONE));
  slope = division.quotient;

  return start_y +
         ((slope * (lateral_position - start_x)) >>
          ROAD_PROFILE_INTERPOLATION_FRACTION_BITS);
}
