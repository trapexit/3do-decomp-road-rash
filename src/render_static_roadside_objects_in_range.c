#include "roadside_object_rendering.h"

int
render_static_roadside_objects_in_range(RoadRenderSide *road_side,
                                        unsigned int    visibility_mask,
                                        int             range_start,
                                        int             range_end)
{
  RoadSegmentLaneRuntime *lane;
  StaticRoadsideObject *object;
  int object_index;
  int count;

  lane = road_side->lane;
  count = lane->static_object_count;
  if(count > ROADSIDE_STATIC_OBJECT_COUNT)
    {
      count = ROADSIDE_STATIC_OBJECT_COUNT;
    }
  if(count <= 0)
    {
      return count;
    }

  object = &lane->static_objects[0];
  object_index = 0;
  do
    {
      if((visibility_mask & (1U << object->visibility_group)) != 0 &&
         range_start <= object->lateral_position &&
         object->lateral_position < range_end)
        {
          render_static_roadside_object_cel(road_side, object_index);

          // Even failed renders clamp visible records; untouched bounds stay.
          if(object->collision_box_count > ROAD_OBJECT_COLLISION_BOX_COUNT)
            {
              object->collision_box_count = ROAD_OBJECT_COLLISION_BOX_COUNT;
            }
        }

      object_index++;
      object++;
    } while(object_index < count);

  return count;
}
