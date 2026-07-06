#include "ai_collision_avoidance_runtime.h"

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

enum TrackPositionBearingValue
{
  TRACK_BEARING_FRACTION_BITS = 8,
  TRACK_BEARING_ROUNDING_BIAS = 0xFF,
  TRACK_BEARING_VALUE_MASK = 0x00FFFFFF,
  TRACK_BEARING_SIGN_BIT = 0x00800000
};

#define TRACK_BEARING_SIGN_EXTENSION 0xFF000000U

int
calculate_track_position_bearing(const TrackObjectPosition *origin,
                                 const TrackObjectPosition *target)
{
  int track_delta;
  int lateral_delta;
  int result;

  track_delta = target->track_position - origin->track_position;
  lateral_delta =
    target->lateral_position - origin->lateral_position;
  if(lateral_delta < 0)
    {
      lateral_delta += TRACK_BEARING_ROUNDING_BIAS;
    }
  lateral_delta >>= TRACK_BEARING_FRACTION_BITS;

  result = fixed_vector_angle_24(track_delta, lateral_delta);
  result &= TRACK_BEARING_VALUE_MASK;
  if(result >= TRACK_BEARING_SIGN_BIT)
    {
      result += TRACK_BEARING_SIGN_EXTENSION;
    }

  return result;
}


#undef TRACK_BEARING_SIGN_EXTENSION
