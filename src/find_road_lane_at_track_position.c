#include "road_segment_runtime.h"
#include "rw_semantic_data.h"

enum RoadLaneSearchValue
{
  ROAD_LANE_SEARCH_FRACTION_BITS = 8,
  ROAD_LANE_SEARCH_FRACTION_MASK = 0xFF,
  ROAD_LANE_SEARCH_ROUNDING_BIAS = 0xFF,
  ROAD_LANE_SEARCH_LINK = 2
};

#define ROAD_LANE_TRACK_CELL(track_position)                             \
        (((track_position) +                                                \
          ((track_position) < 0 ? ROAD_LANE_SEARCH_ROUNDING_BIAS : 0)) >>   \
         ROAD_LANE_SEARCH_FRACTION_BITS)

RoadSegmentLaneRuntime *
find_road_lane_at_track_position(int                     track_position,
                                 RoadSegmentLaneRuntime *lane)
{
  const RoadSegmentRuntime *segment;
  int traversed_count;
  int right_bound;
  int left_bound;
  int current_cell;
  int current_position;
  int target_cell;
  int traversal_limit;

  traversed_count = 0;
  right_bound = track_runtime_globals.right_bound;
  if((right_bound & ~ROAD_LANE_SEARCH_FRACTION_MASK) <= track_position)
    {
      return lane;
    }

  left_bound = track_runtime_globals.left_bound;
  if((left_bound & ~ROAD_LANE_SEARCH_FRACTION_MASK) > track_position)
    {
      return lane;
    }

  segment = (const RoadSegmentRuntime *)lane->resource_handle;
  current_position = segment->track_position;
  current_cell = ROAD_LANE_TRACK_CELL(current_position);
  traversal_limit = current_cell -
                    ROAD_LANE_TRACK_CELL(left_bound);
  target_cell = ROAD_LANE_TRACK_CELL(track_position);

  if(current_cell > target_cell)
    {
      do
        {
          lane = lane->next_links[ROAD_LANE_SEARCH_LINK];
          traversed_count++;
          if(traversed_count >= traversal_limit)
            {
              break;
            }
          if(lane->next_links[ROAD_LANE_SEARCH_LINK] == lane)
            {
              break;
            }
          segment = (const RoadSegmentRuntime *)lane->resource_handle;
          current_position = segment->track_position;
          current_cell = ROAD_LANE_TRACK_CELL(current_position);
        } while(current_cell > target_cell);

      if(traversed_count != 0)
        {
          return lane;
        }
    }

  right_bound = ROAD_LANE_TRACK_CELL(right_bound);
  segment = (const RoadSegmentRuntime *)lane->resource_handle;
  current_position = segment->track_position;
  current_cell = ROAD_LANE_TRACK_CELL(current_position);
  traversal_limit = (right_bound - 1) - current_cell;

  if(current_cell >= target_cell)
    {
      return lane;
    }

  do
    {
      lane = lane->previous_links[ROAD_LANE_SEARCH_LINK];
      traversed_count++;
      if(traversed_count >= traversal_limit)
        {
          break;
        }
      if(lane->previous_links[ROAD_LANE_SEARCH_LINK] == lane)
        {
          break;
        }
      segment = (const RoadSegmentRuntime *)lane->resource_handle;
      current_position = segment->track_position;
      current_cell = ROAD_LANE_TRACK_CELL(current_position);
    } while(current_cell < target_cell);

  return lane;
}


#undef ROAD_LANE_TRACK_CELL
