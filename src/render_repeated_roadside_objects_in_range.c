#include "roadside_object_rendering.h"

int
render_repeated_roadside_objects_in_range(RoadRenderSide *road_side,
                                          int             range_start,
                                          int             range_end)
{
  RepeatedRoadsideObjectGroup *group;
  int placement_index;
  int lateral_position;
  int count;

  group = &road_side->lane->repeated_objects;
  count = group->placement_count;
  if(count > ROAD_OBJECT_PLACEMENT_COUNT)
    {
      count = ROAD_OBJECT_PLACEMENT_COUNT;
    }
  if(count <= 0)
    {
      return count;
    }

  placement_index = 0;
  do
    {
      lateral_position = group->lateral_positions[placement_index];
      if(range_start <= lateral_position &&
         lateral_position < range_end)
        {
          render_repeated_roadside_object_cel(road_side, placement_index);
        }
      placement_index++;
    } while(placement_index < count);

  return count;
}
