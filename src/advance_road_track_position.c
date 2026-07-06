
#include "bss_late_data.h"
#include "road_geometry.h"
#include "road_lane_schedule_runtime.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

void
advance_road_track_position(int track_delta)
{
  int previous_left_bound;
  int previous_right_bound;
  int mode;

  if(track_delta == 0)
    {
      return;
    }

  previous_right_bound = track_runtime_globals.right_bound;
  previous_left_bound = track_runtime_globals.left_bound;

  track_runtime_globals.left_bound = previous_left_bound + track_delta;
  track_runtime_globals.right_bound = previous_right_bound + track_delta;
  track_runtime_globals.current_track_position += track_delta;

  if((track_runtime_globals.left_bound >>
      TRACK_POSITION_FRACTION_BITS) !=
     (previous_left_bound >> TRACK_POSITION_FRACTION_BITS))
    {
      if(track_delta > 0)
        {
          advance_road_track_traversal(
            &gRoadTrackReverseTraversal, TRACK_POSITION_UNIT);
          advance_road_lane_schedule_bucket(
            ROAD_TRACK_FIRST_CHANNEL, TRACK_POSITION_UNIT);
          populate_and_link_road_segment_runtime(
            previous_right_bound, &gRoadTrackForwardTraversal);
          advance_road_lane_schedule_bucket(
            ROAD_TRACK_SECOND_CHANNEL, TRACK_POSITION_UNIT);
          advance_road_track_traversal(
            &gRoadTrackForwardTraversal, TRACK_POSITION_UNIT);
        }
      else
        {
          advance_road_track_traversal(
            &gRoadTrackReverseTraversal, -TRACK_POSITION_UNIT);
          advance_road_lane_schedule_bucket(
            ROAD_TRACK_SECOND_CHANNEL, -TRACK_POSITION_UNIT);
          populate_and_link_road_segment_runtime(
            track_runtime_globals.left_bound,
            &gRoadTrackReverseTraversal);
          advance_road_lane_schedule_bucket(
            ROAD_TRACK_FIRST_CHANNEL, -TRACK_POSITION_UNIT);
          advance_road_track_traversal(
            &gRoadTrackForwardTraversal, -TRACK_POSITION_UNIT);
        }
    }

  mode = gRoadSegmentEffectWorkspace.segment_index
         .by_track_index[
    (track_runtime_globals.current_track_position >>
     TRACK_POSITION_FRACTION_BITS) & ROAD_TRACK_INDEX_MASK]
         ->transition_type;
  if(track_delta > 0)
    {
      if(mode == ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY)
        {
          track_runtime_globals.selector_override = 0;
        }
    }
  else if(mode ==
          ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY)
    {
      track_runtime_globals.selector_override = 0;
    }
}
