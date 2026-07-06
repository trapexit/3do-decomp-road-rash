#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00021530 entry of the original function at 0x000213E8. */

void
seek_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                               int                     target_position)
{
  int delta;

  delta = target_position - lane_state->track_position;
  if(delta > 0)
    {
      while(delta >= TRACK_POSITION_UNIT)
        {
          advance_road_lane_width_traversal(
            lane_state, TRACK_POSITION_UNIT);
          delta -= TRACK_POSITION_UNIT;
        }
      advance_road_lane_width_traversal(lane_state, delta);
      return;
    }

  if(delta >= 0)
    {
      return;
    }
  while(delta <= -TRACK_POSITION_UNIT)
    {
      advance_road_lane_width_traversal(
        lane_state, -TRACK_POSITION_UNIT);
      delta += TRACK_POSITION_UNIT;
    }
  advance_road_lane_width_traversal(lane_state, delta);
}
