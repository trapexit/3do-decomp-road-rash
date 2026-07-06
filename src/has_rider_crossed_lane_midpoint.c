/* Reconstructed from the original function at 0x00026F34. */

#include "rider_visual_runtime.h"
#include "road_geometry.h"

enum RiderLaneMidpointConstant
{
  RIDER_LANE_SIGN_BIT_SHIFT =
    TRACK_OBJECT_BITS_PER_BYTE * sizeof(int) - 1,
  RIDER_LANE_POSITION_FRACTION_BITS =
    TRACK_OBJECT_POSITION_FRACTION_BITS,
  RIDER_LANE_BOOLEAN_MASK = TRACK_OBJECT_DIRECTION_MASK
};

static
int
div2_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RIDER_LANE_SIGN_BIT_SHIFT)) >> 1;
}


int
has_rider_crossed_lane_midpoint(const RiderVisualObject *rider)
{
  int position;
  const RoadSegmentLaneRuntime *lane;
  int midpoint;

  position = rider->position.lateral_position;
  lane = rider->track_segment;

  if(rider->lane_direction_index == ROAD_SIDE_RIGHT)
    {
      midpoint = div2_towards_zero(
        lane->shoulder_right + lane->road_right);
      return (position >
              (midpoint << RIDER_LANE_POSITION_FRACTION_BITS)) &
             RIDER_LANE_BOOLEAN_MASK;
    }

  midpoint = div2_towards_zero(
    lane->shoulder_left + lane->road_left);
  return (position <
          (midpoint << RIDER_LANE_POSITION_FRACTION_BITS)) &
         RIDER_LANE_BOOLEAN_MASK;
}
