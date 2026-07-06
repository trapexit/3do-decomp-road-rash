#include "rw_semantic_data.h"

enum FixedTangentConstant
{
  FIXED_TANGENT_HALF_TURN_MASK = 0x1FF,
  FIXED_TANGENT_DIAGONAL_INDEX = 0x80,
  FIXED_TANGENT_QUARTER_TURN_INDEX = 0x100,
  FIXED_TANGENT_QUARTER_TURN_MASK = 0xFF
};

int
fixed_tangent_8_8(int angle_index)
{
  const int *high_lookup;

  if(angle_index < 0)
    {
      return -fixed_tangent_8_8(-angle_index);
    }

  angle_index = (unsigned int)angle_index &
                FIXED_TANGENT_HALF_TURN_MASK;
  if(angle_index < FIXED_TANGENT_DIAGONAL_INDEX)
    {
      return display_runtime_globals.trig_lookup_low[angle_index];
    }
  if(angle_index <= FIXED_TANGENT_QUARTER_TURN_INDEX)
    {
      high_lookup = display_runtime_globals.trig_lookup_high -
                    FIXED_TANGENT_DIAGONAL_INDEX;
      return high_lookup[angle_index];
    }

  return -fixed_tangent_8_8(
    FIXED_TANGENT_QUARTER_TURN_INDEX -
    (angle_index & FIXED_TANGENT_QUARTER_TURN_MASK));
}
