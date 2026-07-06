#include "bss_late_data.h"

/* Reconstructed from the original function at 0x000200AC. */

enum RoadsideObjectSurfaceValue
{
  ROAD_OBJECT_SURFACE_SAMPLE_FRACTION = 0x80,
  ROAD_OBJECT_SURFACE_UNAVAILABLE = 0x3E7
};

static
int
normalize_roadside_object_height(int height)
{
  if(height >= ROAD_OBJECT_SURFACE_UNAVAILABLE)
    {
      return 0;
    }
  return height;
}


void
populate_roadside_object_surface_heights(RoadSegmentLaneRuntime *lane_runtime)
{
  RepeatedRoadsideObjectGroup *repeated;
  StaticRoadsideObject *static_object;
  signed char index;
  int height;

  repeated = &lane_runtime->repeated_objects;
  index = 0;
  while(index < repeated->placement_count)
    {
      height = sample_road_cross_section_height(
        lane_runtime,
        repeated->lateral_positions[index] * TRACK_POSITION_UNIT,
        ROAD_OBJECT_SURFACE_SAMPLE_FRACTION);
      repeated->surface_heights[index] =
        normalize_roadside_object_height(height);
      index = (signed char)(index + 1);
    }

  index = 0;
  while(index < lane_runtime->static_object_count)
    {
      static_object = &lane_runtime->static_objects[index];
      height = sample_road_cross_section_height(
        lane_runtime,
        static_object->lateral_position * TRACK_POSITION_UNIT,
        ROAD_OBJECT_SURFACE_SAMPLE_FRACTION);
      static_object->surface_height =
        normalize_roadside_object_height(height);
      index = (signed char)(index + 1);
    }
}
