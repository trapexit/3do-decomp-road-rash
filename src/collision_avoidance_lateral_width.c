#include "ai_collision_avoidance_runtime.h"

enum CollisionAvoidanceWidthValue
{
  COLLISION_WIDTH_FLAG_NARROW_A = 1,
  COLLISION_WIDTH_FLAG_NARROW_B = 2,
  COLLISION_WIDTH_FLAG_STANDARD_A = 4,
  COLLISION_WIDTH_FLAG_STANDARD_B = 8,
  COLLISION_WIDTH_FLAG_BLOCKING = 0x10,
  COLLISION_WIDTH_FLAG_STANDARD_C = 0x20,
  COLLISION_WIDTH_DEFAULT = 0x1000,
  COLLISION_WIDTH_NARROW = 0x2000,
  COLLISION_WIDTH_STANDARD = 0x3000,
  COLLISION_WIDTH_BLOCKING = 0x8000
};

int
collision_avoidance_lateral_width(const TrackInteractionObject *object)
{
  int flags;

  flags = object->collision_flags;
  if(flags == COLLISION_WIDTH_FLAG_STANDARD_B ||
     flags == COLLISION_WIDTH_FLAG_STANDARD_A ||
     flags == COLLISION_WIDTH_FLAG_STANDARD_C)
    {
      return COLLISION_WIDTH_STANDARD;
    }
  if(flags == COLLISION_WIDTH_FLAG_NARROW_A ||
     flags == COLLISION_WIDTH_FLAG_NARROW_B)
    {
      return COLLISION_WIDTH_NARROW;
    }
  if(flags == COLLISION_WIDTH_FLAG_BLOCKING)
    {
      return COLLISION_WIDTH_BLOCKING;
    }
  return COLLISION_WIDTH_DEFAULT;
}
