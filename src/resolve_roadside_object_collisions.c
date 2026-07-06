#include "roadside_collision_runtime.h"
#include "roadside_object_rendering.h"

enum RoadsideObjectCollisionConstant
{
  ROADSIDE_OBJECT_COLLISION_DISABLED = 0,
  ROADSIDE_OBJECT_COLLISION_LOW = 1,
  ROADSIDE_OBJECT_COLLISION_MEDIUM = 2,
  ROADSIDE_OBJECT_COLLISION_PROXY_MINIMUM = 3,
  ROADSIDE_OBJECT_LOW_HEIGHT = 0x2000,
  ROADSIDE_OBJECT_MEDIUM_HEIGHT = 0x3FFF,
  ROADSIDE_OBJECT_WORLD_SHIFT = 8
};

static
int
roadside_lateral_world_offset(int lateral_position)
{
  /* The ARM oracle uses LSL #8.  Convert through unsigned arithmetic so
     negative roadside positions retain the same 32-bit result without a
     C signed-left-shift violation. */
  return (int)((unsigned int)lateral_position <<
               ROADSIDE_OBJECT_WORLD_SHIFT);
}


static
int
add_roadside_world_offset(int coordinate,
                          int offset)
{
  return (int)((unsigned int)coordinate + (unsigned int)offset);
}


static
int
clamp_roadside_count(int count,
                     int capacity)
{
  if(count < 0)
    {
      return 0;
    }
  if(count > capacity)
    {
      return capacity;
    }
  return count;
}


void
resolve_static_roadside_object_collision(RacerEntity          *racer,
                                         StaticRoadsideObject *object,
                                         RoadObjectBounds     *bounds)
{
  RacerCollisionProxy proxy;
  unsigned int collision_kind;

  if((racer->collision_class &
      RACER_COLLISION_CLASS_IGNORE_STANDARD_OBJECTS) != 0 &&
     object->visibility_group != ROADSIDE_OBJECT_COLLISION_DISABLED)
    {
      return;
    }

  collision_kind = object->visibility_group;
  if(collision_kind < ROADSIDE_OBJECT_COLLISION_PROXY_MINIMUM)
    {
      if(collision_kind == ROADSIDE_OBJECT_COLLISION_LOW)
        {
          bounds->top = bounds->bottom - ROADSIDE_OBJECT_LOW_HEIGHT;
        }
      else if(collision_kind == ROADSIDE_OBJECT_COLLISION_MEDIUM)
        {
          bounds->top = bounds->bottom - ROADSIDE_OBJECT_MEDIUM_HEIGHT;
        }

      resolve_roadside_collision_bounds(
        racer, object->lateral_position, bounds);
      return;
    }

  proxy.track_contact = (TrackContactDescriptor *)object;
  proxy.contact_callback = 0;
  proxy.collision_class = ROADSIDE_COLLISION_CLASS_STATIC_OBJECT;
  racer->contact = (RacerEntity *)&proxy;

  if(racer->contact_callback != 0)
    {
      racer->contact_callback(racer);
    }
  racer->contact = 0;
}


int
check_roadside_object_collisions(RacerEntity *racer)
{
  RoadSegmentLaneRuntime *lane;
  RepeatedRoadsideObjectGroup *repeated;
  StaticRoadsideObject *object;
  StaticRoadsideObject *selected_static_object;
  RoadObjectBounds collision_bounds;
  const RoadObjectBounds *box;
  int left;
  int right;
  int top;
  int bottom;
  int object_index;
  int bounds_index;
  int lateral_offset;
  int count;
  int collision_box_count;
  int collision_found;
  int static_collision_found;
  int selected_lateral_position;

  collision_found = 0;
  static_collision_found = 0;
  selected_static_object = 0;
  selected_lateral_position = 0;
  lane = racer->primary_track;
  repeated = &lane->repeated_objects;
  if(repeated->placement_count == 0 && lane->static_object_count == 0)
    {
      return 0;
    }

  left = racer->position_x - racer->collision_half_width_x;
  right = racer->position_x + racer->collision_half_width_x;
  top = racer->position_y - racer->collision_half_height_y;
  bottom = racer->position_y + racer->collision_half_height_y;

  collision_box_count = clamp_roadside_count(
    repeated->collision_box_count, ROAD_OBJECT_COLLISION_BOX_COUNT);
  count = clamp_roadside_count(
    repeated->placement_count, ROAD_OBJECT_PLACEMENT_COUNT);
  for(object_index = 0; object_index < count; object_index++)
    {
      lateral_offset = roadside_lateral_world_offset(
        repeated->lateral_positions[object_index]);
      for(bounds_index = 0;
          bounds_index < collision_box_count;
          bounds_index++)
        {
          box = &repeated->bounds[bounds_index];
          if(add_roadside_world_offset(box->left, lateral_offset) <=
             right &&
             left <= add_roadside_world_offset(
               box->right, lateral_offset) &&
             box->top <= bottom && top < box->bottom)
            {
              collision_bounds = *box;
              collision_bounds.left = add_roadside_world_offset(
                collision_bounds.left, lateral_offset);
              collision_bounds.right = add_roadside_world_offset(
                collision_bounds.right, lateral_offset);
              selected_lateral_position =
                repeated->lateral_positions[object_index];
              collision_found = 1;
              goto collision_scan_complete;
            }
        }
    }

  count = clamp_roadside_count(
    lane->static_object_count, ROADSIDE_STATIC_OBJECT_COUNT);
  object = &lane->static_objects[0];
  for(object_index = 0; object_index < count; object_index++)
    {
      collision_box_count = clamp_roadside_count(
        object->collision_box_count, ROAD_OBJECT_COLLISION_BOX_COUNT);
      lateral_offset = roadside_lateral_world_offset(
        object->lateral_position);
      for(bounds_index = 0;
          bounds_index < collision_box_count;
          bounds_index++)
        {
          box = &object->bounds[bounds_index];
          if(add_roadside_world_offset(box->left, lateral_offset) <=
             right &&
             left <= add_roadside_world_offset(
               box->right, lateral_offset) &&
             box->top <= bottom && top < box->bottom)
            {
              collision_bounds = *box;
              collision_bounds.left = add_roadside_world_offset(
                collision_bounds.left, lateral_offset);
              collision_bounds.right = add_roadside_world_offset(
                collision_bounds.right, lateral_offset);
              selected_static_object = object;
              collision_found = 1;
              static_collision_found = 1;
              goto collision_scan_complete;
            }
        }
      object++;
    }

collision_scan_complete:
  if(collision_found == 0)
    {
      return 0;
    }
  if(static_collision_found != 0)
    {
      resolve_static_roadside_object_collision(
        racer, selected_static_object, &collision_bounds);
    }
  else
    {
      resolve_roadside_collision_bounds(
        racer, selected_lateral_position, &collision_bounds);
    }
  return 1;
}
