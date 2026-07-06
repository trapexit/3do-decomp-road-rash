#include "race_object_lifecycle.h"
#include "road_geometry.h"
#include "road_segment_runtime.h"
#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"
#include "track_object_runtime.h"

enum TrackObjectActivityConstant
{
  TRACK_OBJECT_VERTICAL_VELOCITY_INTEGER_BYTE = 1
};

static
int
divide_by_two_towards_zero(int value)
{
  return (value + (int)((unsigned int)value >>
                        (sizeof(unsigned int) *
                         TRACK_OBJECT_BITS_PER_BYTE - 1))) >> 1;
}


static
int
divide_by_track_position_unit_towards_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_OBJECT_POSITION_UNIT - 1;
    }
  return value >> TRACK_OBJECT_POSITION_FRACTION_BITS;
}


static
int
inherited_vertical_velocity_has_integer_part(const TrackObject *object)
{
  const unsigned char *vertical_velocity;

  vertical_velocity =
    (const unsigned char *)&object->inherited_velocity.vertical;
  return vertical_velocity[
    TRACK_OBJECT_VERTICAL_VELOCITY_INTEGER_BYTE] != 0;
}


int
track_object_remains_active(TrackObject *object)
{
  int removed_immediately;
  RoadSegmentLaneRuntime *lane;
  RoadJunctionRuntime *junction;
  int crossing_boundary;
  int lateral_position;

  removed_immediately = 0;
  if((object->mode & TRACK_OBJECT_MODE_DIRECTION_BIT) != 0)
    {
      if(object->position.track_position <
         track_runtime_globals.right_bound &&
         object->position.track_position >=
         track_runtime_globals.left_bound)
        {
          lane = object->track_segment;
          if((lane->surface_flags &
              ROAD_SURFACE_FLAG_BLEND_TRANSITION) == 0 ||
             object->inherited_velocity.lateral == 0 ||
             !inherited_vertical_velocity_has_integer_part(object))
            {
              removed_immediately = 1;
              schedule_race_object_removal(object);
            }
        }

      lane = object->track_segment;
      junction = (RoadJunctionRuntime *)lane->resource_handle;
      if(junction->junction_kind ==
         SEGMENT_TOPOLOGY_SECONDARY_JUNCTION)
        {
          lateral_position = divide_by_track_position_unit_towards_zero(
            object->position.lateral_position);
          if(object->mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
            {
              if((lane->topology_flags &
                  ROAD_LANE_TOPOLOGY_PRIMARY) != 0)
                {
                  crossing_boundary = lane->road_right +
                                      divide_by_two_towards_zero(
                    junction->crossing_width);
                  if(crossing_boundary < lateral_position)
                    {
                      schedule_race_object_removal(object);
                    }
                }
            }
          else if(object->mode ==
                  TRACK_OBJECT_MODE_CROSS_NEGATIVE)
            {
              if((lane->topology_flags &
                  ROAD_LANE_TOPOLOGY_SECONDARY) != 0)
                {
                  crossing_boundary = lane->road_left -
                                      divide_by_two_towards_zero(
                    junction->crossing_width);
                  if(crossing_boundary > lateral_position)
                    {
                      schedule_race_object_removal(object);
                    }
                }
            }
        }
    }

  return removed_immediately == 0;
}
