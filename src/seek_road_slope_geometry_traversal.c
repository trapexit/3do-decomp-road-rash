#include "track_traversal_runtime.h"

/* Reconstructed from the 0x0002195C entry of the original function at 0x0002176C. */

void
seek_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                   int                     target_position)
{
  int delta;

  delta = target_position - slope_state->track_position;
  if(delta > 0)
    {
      while(delta >= TRACK_POSITION_UNIT)
        {
          advance_road_slope_geometry_traversal(
            slope_state, TRACK_POSITION_UNIT);
          delta -= TRACK_POSITION_UNIT;
        }
      advance_road_slope_geometry_traversal(slope_state, delta);
    }
  else if(delta < 0)
    {
      while(delta <= -TRACK_POSITION_UNIT)
        {
          advance_road_slope_geometry_traversal(
            slope_state, -TRACK_POSITION_UNIT);
          delta += TRACK_POSITION_UNIT;
        }
      advance_road_slope_geometry_traversal(slope_state, delta);
    }
}
