#include "bss_late_data.h"
#include "road_geometry.h"

enum RoadSurfaceProfilePointIndex
{
  ROAD_SURFACE_PROFILE_POINT_ZERO = 0,
  ROAD_SURFACE_PROFILE_POINT_ONE = 1,
  ROAD_SURFACE_PROFILE_POINT_TWO = 2,
  ROAD_SURFACE_PROFILE_POINT_THREE = 3,
  ROAD_SURFACE_PROFILE_POINT_FOUR = 4,
  ROAD_SURFACE_PROFILE_POINT_FIVE = 5
};

void
reconcile_linked_road_lane_surface_profile(RoadSegmentLaneRuntime *source_lane,
                                           RoadSegmentLaneRuntime *linked_lane)
{
  int source_mode;
  int linked_mode;

  if(linked_lane->clip_position != 0)
    {
      return;
    }

  source_mode = source_lane->geometry_mode;
  linked_mode = linked_lane->geometry_mode;
  if(source_mode == linked_mode)
    {
      return;
    }

  if(linked_mode != ROAD_GEOMETRY_MODE_TEXTURED)
    {
      if(linked_mode != ROAD_GEOMETRY_MODE_PROFILE ||
         source_mode != ROAD_GEOMETRY_MODE_TEXTURED)
        {
          return;
        }
      if(source_lane->surface_flags == 0)
        {
          return;
        }
      source_lane->surface_resource_ids[0] =
        ROAD_SURFACE_RESOURCE_MISSING;
      source_lane->surface_resource_ids[1] =
        ROAD_SURFACE_RESOURCE_MISSING;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ZERO].y = 0;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_TWO].y = 0;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FOUR].y = 0;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ONE].y = 0;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_THREE].y = 0;
      linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FIVE].y = 0;
      return;
    }

  if(linked_lane->surface_flags == 0)
    {
      return;
    }
  linked_lane->geometry_mode = source_lane->geometry_mode;
  linked_lane->surface_resource_ids[0] =
    source_lane->surface_resource_ids[0];
  linked_lane->surface_resource_ids[1] =
    source_lane->surface_resource_ids[1];
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ZERO].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_TWO].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FOUR].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ONE].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_THREE].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FIVE].y = 0;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ZERO].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ZERO].x;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_TWO].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_TWO].x;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FOUR].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FOUR].x;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ONE].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_ONE].x;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_THREE].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_THREE].x;
  linked_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FIVE].x =
    source_lane->surface_profile[ROAD_SURFACE_PROFILE_POINT_FIVE].x;
}
