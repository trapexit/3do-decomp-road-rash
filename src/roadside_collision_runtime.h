#ifndef ROADRASH_ROADSIDE_COLLISION_RUNTIME_H
#define ROADRASH_ROADSIDE_COLLISION_RUNTIME_H

#include "bss_late_data.h"
#include "racer_runtime.h"

enum RoadsideCollisionClassValue
{
  ROADSIDE_COLLISION_CLASS_BOUNDS = 1,
  ROADSIDE_COLLISION_CLASS_STATIC_OBJECT = 2,
  RACER_COLLISION_CLASS_IGNORE_STANDARD_OBJECTS = 0x10
};

/*
 * Stack-local collision target accepted by resolve_racer_entity_collision().
 * The original routines allocate only the collision-relevant 0x1C0-byte
 * prefix of RacerEntity, so this type intentionally ends before owner state.
 */
typedef struct RacerCollisionProxy
{
  unsigned char reserved_000[0x1C];
  int position_z;
  int position_y;
  int position_aux;
  int position_x;
  RoadSegmentLaneRuntime *track_segment;
  int previous_position_z;
  int previous_position_y;
  int previous_position_aux;
  int previous_position_x;
  RoadSegmentLaneRuntime *previous_track_segment;
  unsigned char reserved_044[0x80];
  int collision_class;
  unsigned char reserved_0c8[0x24];
  RacerVelocity velocity;
  unsigned char reserved_0f8[0x0C];
  int lateral_velocity_sample;
  RacerVelocity world_velocity;
  int drive_acceleration;
  int steering_force;
  int surface_acceleration_min;
  int surface_acceleration_max;
  TrackContactDescriptor *track_contact;
  int heading;
  int left_surface_bound;
  int right_surface_bound;
  int left_collision_bound;
  int right_collision_bound;
  RoadSegmentLaneRuntime *secondary_track;
  RoadSegmentLaneRuntime *primary_track;
  RacerEntity *collision_peer;
  unsigned char reserved_148[0x0C];
  int base_drag;
  int surface_drag;
  int drag_limit;
  int surface_contact_scale_8_8;
  unsigned char slip_amount;
  unsigned char surface_contact_alignment[3];
  int minimum_acceleration;
  int contact_threshold;
  int bounce_scale;
  int collision_half_width_x;
  int collision_half_length_z;
  int collision_half_height_y;
  int collision_impulse_scale_8_8;
  RacerVelocity contact_impulse;
  int other_collision_mass;
  int collision_mass;
  int collision_scratch;
  unsigned char collision_flags;
  unsigned char collision_alignment[3];
  RacerEntity *contact;
  RacerContactCallback contact_callback;
  unsigned char reserved_1a8[0x18];
} RacerCollisionProxy;

typedef char RacerCollisionProxySizeCheck[
    sizeof(RacerCollisionProxy) == 0x1C0 ? 1 : -1];
typedef char RacerCollisionProxyTrackContactOffsetCheck[
    offsetof(RacerCollisionProxy, track_contact) == 0x124 ? 1 : -1];
typedef char RacerCollisionProxyExtentsOffsetCheck[
    offsetof(RacerCollisionProxy, collision_half_width_x) == 0x174 ? 1 : -1];
typedef char RacerCollisionProxyContactOffsetCheck[
    offsetof(RacerCollisionProxy, contact) == 0x1A0 ? 1 : -1];

void
resolve_roadside_collision_bounds(RacerEntity            *racer,
                                  int                     lateral_position,
                                  const RoadObjectBounds *bounds);
void
resolve_static_roadside_object_collision(RacerEntity          *racer,
                                         StaticRoadsideObject *object,
                                         RoadObjectBounds     *bounds);
/* Nonzero when a selected collision may have changed racer state. */
int
check_roadside_object_collisions(RacerEntity *racer);
int
resolve_active_racer_collisions(IntrusiveListHeader *racers);

#endif
