#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

enum RoadLaneOffsetValue
{
  ROAD_LANE_TRACK_FRACTION_BITS = 8,
  ROAD_LANE_TRACK_ROUNDING_BIAS = 0xFF,
  ROAD_LANE_TRAVERSAL_LINK = 2
};

RoadSegmentLaneRuntime *
move_road_lane_by_offset(RoadSegmentLaneRuntime *lane,
                         int                     position_offset)
{
  RoadSegmentRuntime *segment;
  int lane_position;
  int boundary_position;
  int remaining_steps;

  if(position_offset < 0)
    {
      position_offset += ROAD_LANE_TRACK_ROUNDING_BIAS;
    }
  position_offset >>= ROAD_LANE_TRACK_FRACTION_BITS;

  if(position_offset > 0)
    {
      segment = (RoadSegmentRuntime *)lane->resource_handle;
      lane_position =
        segment->track_position >> ROAD_LANE_TRACK_FRACTION_BITS;
      boundary_position = track_runtime_globals.right_bound >>
                          ROAD_LANE_TRACK_FRACTION_BITS;
      if(lane_position + position_offset >= boundary_position)
        {
          position_offset = boundary_position - lane_position - 1;
        }

      remaining_steps = position_offset;
      position_offset--;
      if(remaining_steps == 0)
        {
          return lane;
        }

      do
        {
          lane = lane->previous_links[ROAD_LANE_TRAVERSAL_LINK];
          remaining_steps = position_offset;
          position_offset--;
        } while(remaining_steps != 0);

      return lane;
    }

  if(position_offset >= 0)
    {
      return lane;
    }

  segment = (RoadSegmentRuntime *)lane->resource_handle;
  lane_position = segment->track_position >> ROAD_LANE_TRACK_FRACTION_BITS;
  boundary_position = track_runtime_globals.left_bound >>
                      ROAD_LANE_TRACK_FRACTION_BITS;
  if(lane_position + position_offset < boundary_position)
    {
      position_offset = boundary_position - lane_position;
    }

  remaining_steps = position_offset;
  position_offset++;
  if(remaining_steps == 0)
    {
      return lane;
    }

  do
    {
      lane = lane->next_links[ROAD_LANE_TRAVERSAL_LINK];
      remaining_steps = position_offset;
      position_offset++;
    } while(remaining_steps != 0);

  return lane;
}
