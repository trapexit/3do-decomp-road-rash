#include "stdlib.h"

#include "intrusive_list.h"
#include "rider_animation_runtime.h"
#include "rider_contact_runtime.h"
#include "rider_visual_resource_binding.h"
#include "rider_visual_runtime.h"
#include "road_projected_cel.h"
#include "spatial_audio_event_queue.h"

void
update_passive_racer_motion_and_collisions(RacerEntity *racer);
void
advance_racer_track_position(int object,
                             int distance,
                             int update_lane);

static int
divide_by_sixteen_towards_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_DIVIDE_BY_SIXTEEN_BIAS;
    }
  return value >> RIDER_DIVIDE_BY_SIXTEEN_SHIFT;
}


__inline
static
int
divide_by_256_towards_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_DIVIDE_BY_256_BIAS;
    }
  return value >> RIDER_DIVIDE_BY_256_SHIFT;
}


__inline
static
int
absolute_track_distance(int distance)
{
  return distance < 0 ? -distance : distance;
}


int
destroy_rider_visual_pool(void)
{
  if(rider_visual_pool != 0)
    {
      destroy_intrusive_object_pool(rider_visual_pool);
      rider_visual_pool = 0;
    }
  return 0;
}


void
rider_visual_create(RiderVisualObject *rider,
                    int                bitmap)
{
  int resources_ready;
  int y;
  int z;

  /* rider_visual_create is invoked by the road renderer while its global
     family barrier is held.  Keep all resource reads on that stable
     publication without contending with a loader's group reservation. */
  refresh_rider_visual_resource_binding(rider);
  resources_ready =
    lookup_family_resource_child_for_render(
      rider->family_resource_inventory, RIDER_FAMILY_ITEM_TYPE,
      RIDER_FAMILY_ANIMATION_SLOT) != 0 &&
    lookup_family_resource_child_for_render(
      rider->family_resource_inventory, RIDER_FAMILY_ITEM_TYPE,
      RIDER_FAMILY_CANS_SLOT) != 0;

  if(rider->animation_ready == 0)
    {
      if(resources_ready == 0)
        {
          return;
        }
      initialize_rider_visual_animations(rider);
    }

  if(resources_ready == 0 || rider->animation_ready == 0)
    {
      return;
    }

  prepare_cans_animation_frame_render(
    (CansAnimationRenderObject *)rider,
    (RoadProjectedRenderContext *)bitmap);
  if(rider->render_ccb == 0)
    {
      return;
    }

  y = rider->render_y +
      (rider->render_y_fraction >> RIDER_RENDER_POSITION_SHIFT);
  z = divide_by_256_towards_zero(rider->render_z_fraction);
  render_road_projected_cel(
    (RoadProjectedRenderContext *)bitmap,
    (RoadProjectedCelDescriptor *)&rider->render_ccb,
    RIDER_RENDER_PROJECT_SCALE, z, y, rider->render_x, 1);
}


void
rider_visual_contact(RacerEntity *object)
{
  RiderVisualObject *rider;
  RacerEntity *contact;
  int speed;
  int absolute_speed;
  int delta;
  int new_speed;

  rider = (RiderVisualObject *)object;
  if(try_acquire_family_resource_render_barrier() != 0)
    {
      refresh_rider_visual_resource_binding(rider);
      release_family_resource_render_barrier();
    }
  if(rider->animation_ready == 0)
    {
      return;
    }

  if(rider->behavior_state == RIDER_BEHAVIOR_CONTACT_LOCKED)
    {
      goto finish;
    }

  contact = rider->contact;
  if(contact->collision_class == RIDER_CONTACT_ATTACK_CLASS &&
     is_rider_attack_animation(contact) != 0)
    {
      goto contact_locked;
    }

  speed = contact->velocity.forward;
  absolute_speed = absolute_track_distance(speed);
  if(absolute_speed <= RIDER_CONTACT_MINIMUM_SPEED)
    {
      goto recovering;
    }

  if(contact->collision_class == RIDER_CONTACT_RACER_CLASS &&
     contact->owner->rider_mode == RIDER_CONTACT_RECOVERY_MODE)
    {
      goto recovering;
    }

contact_locked:
  delta = rider->position.track_position -
          track_simulation_globals.motion_source.packed_track_position;
  if(delta >= 0 && delta <= RIDER_CONTACT_SOUND_WINDOW &&
     frame_tick - rider->last_sound_tick > RIDER_SOUND_COOLDOWN_TICKS)
    {
      if(queue_family_transient_spatial_sample_event(
           rider->family_resource_inventory,
           RIDER_FAMILY_ITEM_TYPE,
           RIDER_FAMILY_CONTACT_SAMPLE_SLOT,
           &rider->position.track_position) != 0)
        {
          rider->last_sound_tick = frame_tick;
        }
    }

  rider->behavior_state = RIDER_BEHAVIOR_CONTACT_LOCKED;
  // These bindings belong to the pedestrian's pool slot, not the streamed
  // resource. Commit the fall with the behavior change; it is not retried.
  set_primary_rider_animation(&rider->animation,
                              RIDER_ANIMATION_FALL);
  set_secondary_rider_animation(&rider->animation,
                                RIDER_ANIMATION_NONE);
  rider->contact_delay = RIDER_CONTACT_DELAY_TICKS;
  apply_race_object_heading_and_force(
    (RacerEntity *)(void *)rider, 0,
    RIDER_CONTACT_STOP_FORCE, 0);
  goto finish;

recovering:
  rider->behavior_state = RIDER_BEHAVIOR_RECOVERING;
  if(rider->animation_states[RIDER_ANIMATION_SHAKE] != 0)
    {
      set_primary_rider_animation(&rider->animation,
                                  RIDER_ANIMATION_SHAKE);
    }

  if(rider->animation_states[RIDER_ANIMATION_ATTACK] != 0)
    {
      if(rider->animation_states[RIDER_ANIMATION_FAST] == 0 ||
         (rand() & RIDER_RANDOM_FAST_MASK) != 0)
        {
          set_secondary_rider_animation(
            &rider->animation,
            RIDER_ANIMATION_ATTACK);
          if(frame_tick - rider->last_sound_tick >
             RIDER_SOUND_COOLDOWN_TICKS)
            {
              if(queue_family_transient_spatial_sample_event(
                   rider->family_resource_inventory,
                   RIDER_FAMILY_ITEM_TYPE,
                   RIDER_FAMILY_ATTACK_SAMPLE_SLOT,
                   &rider->position.track_position) != 0)
                {
                  rider->last_sound_tick = frame_tick;
                }
            }
        }
      else
        {
          set_secondary_rider_animation(
            &rider->animation,
            RIDER_ANIMATION_FAST);
        }
    }
  else if(rider->animation_states[RIDER_ANIMATION_FAST] != 0)
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_FAST);
    }
  else
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_STANDING);
    }

finish:
  contact = rider->contact;
  speed = contact->velocity.forward;
  if(rider->behavior_state == RIDER_BEHAVIOR_CONTACT_LOCKED)
    {
      return;
    }

  new_speed = divide_by_sixteen_towards_zero(speed);
  rider->velocity.forward = new_speed;
  if(new_speed < 0)
    {
      rider->velocity.forward = 0;
    }

  rider->movement_heading = 0;
  if(speed <= 0)
    {
      rider->movement_heading = RIDER_REVERSE_MOVEMENT_HEADING;
    }

}


void
rider_visual_destroy(RiderVisualObject *rider)
{
  int delta;
  int distance;
  int step;

  update_passive_racer_motion_and_collisions((RacerEntity *)rider);

  delta = rider->target_position.track_position -
          rider->position.track_position;
  distance = absolute_track_distance(delta);
  if(distance < RIDER_TRACK_SNAP_DISTANCE)
    {
      return;
    }

  if(delta >= 0)
    {
      step = delta - RIDER_TRACK_SNAP_BIAS;
    }
  else
    {
      step = delta + RIDER_TRACK_SNAP_BIAS;
    }
  advance_racer_track_position((int)rider, step, 1);
  rider->velocity.forward = 0;
}
