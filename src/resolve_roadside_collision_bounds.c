#include "roadside_collision_runtime.h"

enum RoadsideBoundsCollisionConstant
{
  ROADSIDE_COLLISION_POSITION_ROUNDING = 0x80,
  ROADSIDE_COLLISION_POSITION_FRACTION_MASK = 0xFF,
  ROADSIDE_COLLISION_LATERAL_SHIFT = 8,
  ROADSIDE_COLLISION_PROXY_MASS = 0x1000000,
  ROADSIDE_COLLISION_PROXY_IMPULSE_SCALE = 0xC0
};

void
resolve_roadside_collision_bounds(RacerEntity            *racer,
                                  int                     lateral_position,
                                  const RoadObjectBounds *bounds)
{
  RacerCollisionProxy proxy;
  int longitudinal_position;

  longitudinal_position =
    (racer->position_z & ~ROADSIDE_COLLISION_POSITION_FRACTION_MASK) +
    ROADSIDE_COLLISION_POSITION_ROUNDING;
  proxy.position_z = longitudinal_position;
  proxy.left_collision_bound = longitudinal_position;
  proxy.right_collision_bound = longitudinal_position;

  proxy.track_segment = racer->primary_track;
  proxy.secondary_track = racer->primary_track;
  proxy.primary_track = racer->primary_track;
  /* resolve_racer_entity_collision() tests this field before any other
     proxy mutation.  The oracle's temporary semantically has no previous
     collision peer; make that required state explicit instead of depending
     on stale task-stack bytes. */
  proxy.collision_peer = 0;

  /* The oracle uses a 32-bit ARM LSL here.  Convert through unsigned
     arithmetic so ordinary left-side (negative) positions retain that
     result without invoking C's signed-left-shift undefined behavior. */
  proxy.position_x = (int)((unsigned int)lateral_position <<
                           ROADSIDE_COLLISION_LATERAL_SHIFT);
  proxy.position_y = 0;
  proxy.position_aux = 0;

  proxy.previous_position_z = proxy.position_z;
  proxy.previous_position_y = proxy.position_y;
  proxy.previous_position_aux = proxy.position_aux;
  proxy.previous_position_x = proxy.position_x;
  proxy.previous_track_segment = proxy.track_segment;

  proxy.velocity.forward = 0;
  proxy.collision_class = ROADSIDE_COLLISION_CLASS_BOUNDS;
  proxy.velocity.vertical = 0;
  proxy.velocity.lateral = 0;
  proxy.world_velocity.forward = 0;
  proxy.world_velocity.vertical = 0;
  proxy.world_velocity.lateral = 0;

  proxy.collision_half_width_x = bounds->right - bounds->left;
  proxy.collision_half_length_z = 0;
  proxy.collision_half_height_y = bounds->bottom - bounds->top;
  proxy.collision_impulse_scale_8_8 =
    ROADSIDE_COLLISION_PROXY_IMPULSE_SCALE;
  proxy.contact_callback = 0;
  proxy.collision_mass = ROADSIDE_COLLISION_PROXY_MASS;

  racer->contact = (RacerEntity *)&proxy;
  resolve_racer_entity_collision(racer, (RacerEntity *)&proxy);

  if(racer->contact_callback != 0)
    {
      racer->contact_callback(racer);
    }
}
