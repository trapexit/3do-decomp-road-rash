/* Reconstructed from the original function at 0x000141C0. */



#include "bss_late_data.h"
#include "racer_runtime.h"
#include "rider_timestep.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"

enum
{
  VECTOR_COMPONENT_COUNT = 3,
  RACER_TYPE_PLAYER = 4,
  RACER_TYPE_LINKED = 8,
  TRACK_KIND_ROAD = 1,
  RACER_DISABLED_TYPE_MASK = 0x20,
  RACER_COLLISION_FLAG = 0x20,
  RACER_NO_CONTACT_FLAG = 0x20,
  RACER_ZONE_DISABLED_FLAG = 0x20,
  RACER_ZONE_EDGE_FLAGS = 0x18,
  RACER_ZONE_KIND_SPECIAL = 3,
  RACER_ZONE_OUTSIDE = 3,
  RACER_ZONE_EDGE = 2,
  RACER_ZONE_INSIDE = 1,
  RACER_SURFACE_CONTACT_PRESENT = 1,
  RACER_ZONE_MARGIN = 0x30,
  RACER_SURFACE_POSITION_FRACTION_BITS = 8,
  SURFACE_SCALE_ROUNDING_BIAS = 0xFF,
  SURFACE_SCALE_SHIFT = 8,
  TRACK_FACTOR_DELTA = 5,
  TRACK_FACTOR_SHIFT = 7,
  SURFACE_DIFFERENCE_ROUNDING_BIAS = 0x800,
  SURFACE_DIFFERENCE_SHIFT = 11,
  LEAN_INTERPOLATION_ROUNDING_BIAS = 0x3F,
  LEAN_INTERPOLATION_SHIFT = 6,
  LEAN_LIMIT = 0x2000,
  COLLISION_FORCE = 0x02000000,
  COLLISION_VECTOR_MAGNITUDE = 0x50,
  COLLISION_RESPONSE_MODE = 2,
  COLLISION_ANIMATION_MODE = 6,
  COLLISION_EVENT = 5,
  COLLISION_EVENT_DURATION = 0xB4,
  PLAYER_BOUNCE_EVENT = 0x1A,
  INITIAL_BOUNCE_LEAN = 0x1000,
  FACTOR_SCALE = 4,
  FACTOR_BIAS = 3,
  FACTOR_FAST = 0x10,
  FACTOR_FAST_SHIFT = 4,
  COLLISION_TRIGGER_THRESHOLD = 0x4600,
  STEERING_SCALE_SHIFT = 12
};

/* Slot proof: 0x11C is surface_acceleration_min (not _max): int chain
 * from velocity@0xEC (RacerVelocityOffsetCheck) through
 * drive_acceleration/steering_force/min in declaration order; see
 * struct RacerEntity in racer_runtime.h. */
typedef char RacerSurfaceAccelerationMinIs11c[
    (offsetof(RacerEntity, surface_acceleration_min) == 0x11C) ? 1 : -1];

int
apply_rider_collision_damage(RacerEntity *rider,
                             int          rider_damage,
                             int          bike_damage,
                             int          recovery_kind);
void
schedule_race_outcome_event(int event,
                            int duration);
void
begin_rider_recovery(RacerEntity         *rider,
                     const RacerVelocity *state_velocity_delta,
                     const RacerVelocity *rider_velocity_delta,
                     int                  recovery_kind);



static
int
trigger_collision(RacerEntity *racer)
{
  RacerEntity *target;
  RoadSegmentLaneRuntime *track;
  int lower[VECTOR_COMPONENT_COUNT];
  int upper[VECTOR_COMPONENT_COUNT];
  int type;

  type = racer->collision_class;
  track = racer->primary_track;
  if(type == RACER_TYPE_PLAYER)
    {
      target = racer;
    }
  else if(type == RACER_TYPE_LINKED &&
          track->geometry_mode == TRACK_KIND_ROAD)
    {
      target = racer->owner;
    }
  else
    {
      return 0;
    }

  if(track->geometry_mode == TRACK_KIND_ROAD)
    {
      apply_rider_collision_damage(target, COLLISION_FORCE,
                                   COLLISION_FORCE, COLLISION_RESPONSE_MODE);
    }
  if(target == (RacerEntity *)race_rider_state.player_object)
    {
      schedule_race_outcome_event(COLLISION_EVENT, COLLISION_EVENT_DURATION);
    }

  lower[0] = 0;
  lower[1] = 0;
  lower[RACER_ZONE_EDGE] = -COLLISION_VECTOR_MAGNITUDE;
  upper[0] = 0;
  upper[1] = 0;
  upper[RACER_ZONE_EDGE] = COLLISION_VECTOR_MAGNITUDE;
  begin_rider_recovery(target, (const RacerVelocity *)lower,
                       (const RacerVelocity *)upper,
                       COLLISION_ANIMATION_MODE);
  return 1;
}


static
void
bounce(RacerEntity *racer,
       int         acceleration)
{
  int product;
  int velocity;
  int offset;

  product = racer->bounce_scale * acceleration;
  if(product < 0)
    {
      product += SURFACE_SCALE_ROUNDING_BIAS;
    }
  velocity = -(product >> SURFACE_SCALE_SHIFT);

  offset = (racer->velocity.forward >> TRACK_FACTOR_SHIFT) *
           (racer->primary_track->path_elevation_step +
            TRACK_FACTOR_DELTA);
  if(racer->velocity.forward < 0)
    {
      velocity -= offset;
    }
  else
    {
      velocity += offset;
    }
  racer->velocity.vertical = velocity;
  racer->surface_acceleration_min =
    -(racer->impact_strength << 1);
  racer->surface_contact_scale_8_8 =
    rider_physics_tuning.steering_heading_reference;
  racer->reserved_0fc = -INITIAL_BOUNCE_LEAN;

  if(racer == (RacerEntity *)race_rider_state.player_object)
    {
      queue_spatial_audio_event(PLAYER_BOUNCE_EVENT,
                                &racer->position_z,
                                &racer->velocity.forward);
    }
}


static
void
update_surface_speed(RacerEntity *racer)
{
  int product;
  int value;

  product =
    racer->velocity.forward *
    (racer->primary_track->path_elevation_step -
     racer->secondary_track->path_elevation_step);
  if(product < 0)
    {
      product -= 1;
      product += SURFACE_DIFFERENCE_ROUNDING_BIAS;
    }
  value = rider_physics_tuning.steering_heading_reference +
          (product >> SURFACE_DIFFERENCE_SHIFT);
  if(value < 0)
    {
      value = 0;
    }
  racer->surface_contact_scale_8_8 = value;
}


static
void
finish_motion(RacerEntity *racer,
              int         value)
{
  int current;
  int delta;

  racer->position_y = value;
  current = racer->reserved_0fc;
  delta = value - current;
  if(delta < 0)
    {
      delta += LEAN_INTERPOLATION_ROUNDING_BIAS;
    }
  current += delta >> LEAN_INTERPOLATION_SHIFT;
  if(current > LEAN_LIMIT)
    {
      current = LEAN_LIMIT;
    }
  else if(current < -LEAN_LIMIT)
    {
      current = -LEAN_LIMIT;
    }
  racer->reserved_0fc = current;
  racer->reserved_100 = current - value;
}

/* lane stays char *: callers pass (char *)track_segment, so no single
 * aggregate parameter type fits the TU boundary; reinterpret once below. */

int
classify_rider_surface_zone(char *lane,
                            int   fixed_position)
{
  int position;
  int lower;
  int upper;
  int flags;
  int kind;
  RoadSegmentLaneRuntime *lane_runtime;

  lane_runtime = (RoadSegmentLaneRuntime *)lane;
  position = fixed_position >> RACER_SURFACE_POSITION_FRACTION_BITS;
  lower = lane_runtime->road_left;
  if(lower < position)
    {
      upper = lane_runtime->road_right;
      if(position < upper)
        {
          return 0;
        }
    }

  flags = lane_runtime->surface_flags;
  if(flags != 0)
    {
      if((flags & RACER_ZONE_DISABLED_FLAG) != 0)
        {
          return 0;
        }
      if((flags & RACER_ZONE_EDGE_FLAGS) != 0)
        {
          return lane_runtime->geometry_mode == 0
                     ? RACER_ZONE_INSIDE
                     : RACER_ZONE_EDGE;
        }
    }

  kind = lane_runtime->geometry_mode;
  if(kind == RACER_ZONE_KIND_SPECIAL)
    {
      return RACER_ZONE_INSIDE;
    }
  upper = lane_runtime->road_right;
  if(upper < position)
    {
      if(lane_runtime->shoulder_right <= position)
        {
          return RACER_ZONE_OUTSIDE;
        }
      if(kind == 0)
        {
          return RACER_ZONE_INSIDE;
        }
      return upper + RACER_ZONE_MARGIN >= position ? 0 : RACER_ZONE_EDGE;
    }

  if(lane_runtime->shoulder_left >= position)
    {
      return RACER_ZONE_OUTSIDE;
    }
  if(kind == 0)
    {
      return RACER_ZONE_INSIDE;
    }
  return lower - RACER_ZONE_MARGIN <= position ? 0 : RACER_ZONE_EDGE;
}


/* racer stays char *: no in-tree caller proves the pointed-to type;
 * reinterpret once below (the twin vertical function takes RacerEntity,
 * but that proof does not transfer here). */
void
update_racer_surface_motion_from_previous_height(char *racer)
{
  int first_position;
  int second_position;
  int old_position;
  int target;
  int factor;
  int acceleration;
  int new_acceleration;
  int raw_position;
  int error;
  int threshold;
  int limit;
  int value;
  RacerEntity *racer_entity;

  racer_entity = (RacerEntity *)racer;
  first_position = sample_road_cross_section_height(
    racer_entity->primary_track,
    racer_entity->position_x,
    (unsigned int)racer_entity->right_collision_bound);
  old_position = racer_entity->position_aux;
  target = (first_position - old_position) <<
           RACER_SURFACE_POSITION_FRACTION_BITS;
  second_position = sample_road_cross_section_height(
    racer_entity->primary_track,
    racer_entity->position_x,
    (unsigned int)racer_entity->right_collision_bound);
  racer_entity->position_aux = second_position;

  target += racer_entity->left_surface_bound;
  factor = racer_entity->time_step;
  acceleration = racer_entity->velocity.vertical;
  raw_position =
    factor * acceleration + racer_entity->position_y;
  if(racer_entity->surface_contact_scale_8_8 != 0)
    {
      raw_position -= target;
    }
  else
    {
      raw_position -= racer_entity->left_surface_bound;
    }
  error = raw_position -
          (second_position << RACER_SURFACE_POSITION_FRACTION_BITS);

  new_acceleration = acceleration +
                     factor *
                     racer_entity->surface_acceleration_min;
  racer_entity->velocity.vertical = new_acceleration;
  threshold = racer_entity->contact_threshold *
              (factor * FACTOR_SCALE - FACTOR_BIAS);
  value = raw_position;

  if(error <= threshold)
    {
      if(racer_entity->position_y -
         (second_position << RACER_SURFACE_POSITION_FRACTION_BITS) >
         threshold)
        {
          if(new_acceleration <
             racer_entity->minimum_acceleration)
            {
              bounce(racer_entity, new_acceleration);
              finish_motion(racer_entity, 0);
              return;
            }
          racer_entity->slip_amount = 1;
        }

      if(factor == 1)
        {
          racer_entity->velocity.vertical = target;
        }
      else if(factor == FACTOR_FAST)
        {
          racer_entity->velocity.vertical =
            target >> FACTOR_FAST_SHIFT;
        }
      else
        {
          racer_entity->velocity.vertical =
            _rider_timestep_quotient(factor, target);
        }
      update_surface_speed(racer_entity);
      value = 0;
    }
  else
    {
      if(racer_entity->collision_class == RACER_TYPE_PLAYER)
        {
          racer_entity->action_flags |= RACER_COLLISION_FLAG;
        }
      if(racer_entity->position_y < threshold)
        {
          limit = racer_entity->impact_strength *
                  racer_entity->impact_scale;
          if(limit < new_acceleration)
            {
              racer_entity->velocity.vertical = limit;
            }
          if(error >= factor * COLLISION_TRIGGER_THRESHOLD &&
             trigger_collision(racer_entity))
            {
              value = 0;
            }
        }
      racer_entity->surface_acceleration_min =
        -racer_entity->impact_strength;
      racer_entity->surface_contact_scale_8_8 = 0;
    }

  finish_motion(racer_entity, value);
}


/* racer stays char *: both typed callers pass (char *)racer (RacerEntity),
 * so retyping would break those TUs; reinterpret once below. */
void
update_racer_vertical_surface_motion(char *racer)
{
  int position;
  int target;
  int factor;
  int acceleration;
  int new_acceleration;
  int raw_position;
  int threshold;
  int limit;
  int magnitude;
  int value;
  RacerEntity *racer_entity;

  racer_entity = (RacerEntity *)racer;
  position = sample_road_cross_section_height(
    racer_entity->primary_track,
    racer_entity->position_x,
    (unsigned int)racer_entity->right_collision_bound);
  racer_entity->position_aux = position;
  if((racer_entity->collision_class &
      RACER_DISABLED_TYPE_MASK) != 0)
    {
      racer_entity->velocity.vertical = 0;
      racer_entity->position_y = 0;
      racer_entity->surface_contact_scale_8_8 =
        rider_physics_tuning.steering_heading_reference;
      return;
    }

  target = racer_entity->left_surface_bound +
           ((position - racer_entity->previous_position_aux) <<
            RACER_SURFACE_POSITION_FRACTION_BITS);
  factor = racer_entity->time_step;
  acceleration = racer_entity->velocity.vertical;
  raw_position =
    factor * acceleration + racer_entity->position_y -
    target;
  new_acceleration = acceleration +
                     factor *
                     racer_entity->surface_acceleration_min;
  racer_entity->velocity.vertical = new_acceleration;
  threshold = racer_entity->contact_threshold *
              (factor * FACTOR_SCALE - FACTOR_BIAS);
  value = raw_position;

  if(raw_position <= threshold)
    {
      if(racer_entity->previous_position_y >
         threshold)
        {
          if(new_acceleration <
             racer_entity->minimum_acceleration)
            {
              if(racer_entity->collision_class ==
                 RACER_TYPE_PLAYER)
                {
                  magnitude = new_acceleration;
                  if(magnitude <= 0)
                    {
                      magnitude = -magnitude;
                    }
                  value = RACER_SURFACE_CONTACT_PRESENT +
                          ((racer_entity->steering_angle *
                            magnitude) >>
                           STEERING_SCALE_SHIFT);
                  racer_entity->steering_angle =
                    value;
                  limit = racer_entity->steering_limit <<
                          1;
                  if(value > limit)
                    {
                      racer_entity->steering_angle =
                        limit;
                    }
                  else if(value < -limit)
                    {
                      racer_entity->steering_angle =
                        -limit;
                    }
                }
              bounce(racer_entity, new_acceleration);
              finish_motion(racer_entity, 0);
              return;
            }
          racer_entity->slip_amount = 1;
        }

      if(factor == 1)
        {
          racer_entity->velocity.vertical = target;
        }
      else if(factor == FACTOR_FAST)
        {
          racer_entity->velocity.vertical =
            target >> FACTOR_FAST_SHIFT;
        }
      else
        {
          racer_entity->velocity.vertical =
            _rider_timestep_quotient(factor, target);
        }
      update_surface_speed(racer_entity);
      value = 0;
    }
  else
    {
      if(racer_entity->collision_class == RACER_TYPE_PLAYER)
        {
          racer_entity->action_flags |= RACER_COLLISION_FLAG;
        }
      if(racer_entity->previous_position_y <
         threshold)
        {
          limit = racer_entity->impact_strength *
                  racer_entity->impact_scale;
          if(limit < new_acceleration)
            {
              racer_entity->velocity.vertical = limit;
            }
          if(raw_position >= factor * COLLISION_TRIGGER_THRESHOLD &&
             trigger_collision(racer_entity))
            {
              value = 0;
            }
        }
      racer_entity->surface_acceleration_min =
        -racer_entity->impact_strength;
      racer_entity->surface_contact_scale_8_8 = 0;
    }

  finish_motion(racer_entity, value);
}
