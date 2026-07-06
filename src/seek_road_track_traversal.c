#include "bss_late_data.h"

void
seek_road_track_traversal(RoadTrackTraversalState *traversal,
                          int                      target_position)
{
  int delta;

  delta = target_position - traversal->track_position;
  if(delta > 0)
    {
      while(delta >= TRACK_POSITION_UNIT)
        {
          advance_road_track_traversal(traversal, TRACK_POSITION_UNIT);
          delta -= TRACK_POSITION_UNIT;
        }
      advance_road_track_traversal(traversal, delta);
    }
  else if(delta < 0)
    {
      while(delta <= -TRACK_POSITION_UNIT)
        {
          advance_road_track_traversal(traversal, -TRACK_POSITION_UNIT);
          delta += TRACK_POSITION_UNIT;
        }
      advance_road_track_traversal(traversal, delta);
    }
}
