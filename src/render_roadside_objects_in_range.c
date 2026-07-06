#include "racer_runtime.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"

enum RoadsideObjectScanConstant
{
  VISIBLE_OBJECT_POSITION_FRACTION_BITS = 8,
  VISIBLE_OBJECT_POSITION_FRACTION_MASK = 0xFF,
  VISIBLE_OBJECT_POSITION_HALF_CELL = 0x80,
  ROAD_OBJECT_MINIMUM_PROJECTION_DEPTH = 0x92,
  ROAD_STATIC_CENTER_VISIBILITY_MASK = 0xFF8
};

#define ROAD_STATIC_OUTER_VISIBILITY_MASK 0xFFFFF007U

__inline
static
int
track_position_to_integer(int position)
{
  if(position < 0)
    {
      position += VISIBLE_OBJECT_POSITION_FRACTION_MASK;
    }
  return position >> VISIBLE_OBJECT_POSITION_FRACTION_BITS;
}


__inline
static
void
render_lane_boundary_objects(RoadRenderSide *road_side,
                             int             range_start,
                             int             range_end)
{
  RoadSegmentLaneRuntime *lane;

  lane = road_side->lane;
  if(lane->static_object_count != 0)
    {
      render_static_roadside_objects_in_range(
        road_side, ROAD_STATIC_OUTER_VISIBILITY_MASK,
        range_start, range_end);
    }
  if(lane->repeated_objects.placement_count != 0)
    {
      render_repeated_roadside_objects_in_range(
        road_side, range_start, range_end);
    }
}


int
render_roadside_objects_in_range(RoadRenderSide *road_side,
                                 int             range_start,
                                 int             range_end)
{
  RoadSegmentLaneRuntime *lane;
  RacerEntity *object;
  int scan_depth;
  int object_index;
  int rendered_boundaries;
  int lateral_position;

  lane = road_side->lane;
  rendered_boundaries = 0;

  if(lane->static_object_count != 0 &&
     road_side->projection_depth > ROAD_OBJECT_MINIMUM_PROJECTION_DEPTH)
    {
      render_static_roadside_objects_in_range(
        road_side, ROAD_STATIC_CENTER_VISIBILITY_MASK,
        range_start, range_end);
    }

  scan_depth = road_side->owner_node->depth;
  object_index = track_visibility_scan.current_index;
  while(object_index >= 0)
    {
      object = gVisibleTrackNodes[object_index];
      if(scan_depth >
         (object->position_snapshot.position_z >>
          VISIBLE_OBJECT_POSITION_FRACTION_BITS))
        {
          break;
        }

      if(object->position_snapshot.track_segment == lane)
        {
          if(!rendered_boundaries &&
             (object->position_snapshot.position_z &
              VISIBLE_OBJECT_POSITION_FRACTION_MASK) <
             VISIBLE_OBJECT_POSITION_HALF_CELL)
            {
              rendered_boundaries = 1;
              if(road_side->projection_depth >
                 ROAD_OBJECT_MINIMUM_PROJECTION_DEPTH)
                {
                  render_lane_boundary_objects(
                    road_side, range_start, range_end);
                }
            }

          lateral_position = track_position_to_integer(
            object->position_snapshot.position_x);
          if(lateral_position >= range_start &&
             lateral_position < range_end &&
             object->render_object != 0)
            {
              object->render_object(object, road_side);
            }
        }

      object_index--;
    }

  if(!rendered_boundaries &&
     road_side->projection_depth > ROAD_OBJECT_MINIMUM_PROJECTION_DEPTH)
    {
      render_lane_boundary_objects(road_side, range_start, range_end);
    }

  track_visibility_scan.resume_index = object_index;
  return (int)&track_visibility_scan.resume_index;
}


#undef ROAD_STATIC_OUTER_VISIBILITY_MASK
