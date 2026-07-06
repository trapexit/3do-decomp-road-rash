#include "division_helpers.h"
#include "road_segment_runtime.h"

enum RoadProfileIntersectionValue
{
  ROAD_PROFILE_INTERSECTION_MIDPOINT_DIVISOR = 2,
  ROAD_PROFILE_INTERSECTION_FRACTION_BITS = 16,
  ROAD_PROFILE_INTERSECTION_FIXED_ONE =
    1 << ROAD_PROFILE_INTERSECTION_FRACTION_BITS
};

int
calculate_road_profile_intersection(const RoadLanePoint *left_start,
                                    const RoadLanePoint *left_end,
                                    const RoadLanePoint *right_start,
                                    const RoadLanePoint *unused_right_end,
                                    RoadLanePoint       *intersection)
{
  SignedDivisionResult division;
  int midpoint;
  int slope;

  (void)unused_right_end;

  midpoint = (left_start->x + right_start->x) /
             ROAD_PROFILE_INTERSECTION_MIDPOINT_DIVISOR;
  division = signed_divide_with_remainder(
    (unsigned int)(left_end->x - left_start->x),
    (unsigned int)((left_end->y - left_start->y) *
                   ROAD_PROFILE_INTERSECTION_FIXED_ONE));
  slope = division.quotient;

  intersection->x = midpoint;
  intersection->y = left_start->y +
                    ((slope * (midpoint - left_start->x)) >>
                     ROAD_PROFILE_INTERSECTION_FRACTION_BITS);
  return 1;
}
