#include "road_geometry.h"
#include "rw_semantic_data.h"

enum RepeatedRoadsideStripCount
{
  REPEATED_ROADSIDE_STRIP_COUNT_DEFAULT = 16,
  REPEATED_ROADSIDE_STRIP_COUNT_DISTANT = 8
};

int
select_repeated_roadside_strip_count(RoadRenderSide *road_side)
{
  int result;

  result = REPEATED_ROADSIDE_STRIP_COUNT_DEFAULT;
  if(road_side->lane->geometry_mode == ROAD_GEOMETRY_MODE_TEXTURED &&
     road_side->owner_node->depth >
     road_renderer_state.texture_resource_threshold)
    {
      result = REPEATED_ROADSIDE_STRIP_COUNT_DISTANT;
    }
  return result;
}
