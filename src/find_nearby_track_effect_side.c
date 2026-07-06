#include "stdlib.h"

#include "family_resource_runtime.h"
#include "rider_animation_runtime.h"
#include "rider_visual_resource_binding.h"
#include "rider_visual_runtime.h"
#include "spatial_audio_event_queue.h"

char *
find_nearby_track_object_in_direction(char *object,
                                      int   effect_mask,
                                      int   search_forward,
                                      int   maximum_distance);
int
has_rider_crossed_lane_midpoint(const RiderVisualObject *rider);

// Animation state and frame bindings belong to the pool slot. Advance them
// independently of the render barrier so rendering cannot stall simulation.

static int
absolute_track_distance(int distance)
{
  return distance < 0 ? -distance : distance;
}


__inline
static
int
rider_lane_index(RiderVisualObject *rider)
{
  return compute_track_lane_index(rider, 0);
}


__inline
static
int
rider_lane_change_is_clear(RiderVisualObject *rider,
                           int                direction,
                           int                lane_offset)
{
  return is_rider_lane_change_clear(rider, direction, lane_offset);
}


int
find_nearby_track_effect_side(RiderVisualObject *rider)
{
  TrackInteractionObject *forward;
  TrackInteractionObject *backward;
  TrackInteractionObject *nearest;
  int effect_type;
  int effect_mask;
  int forward_distance;
  int backward_distance;
  int distance;

  effect_type = rider_lane_index(rider) < 0
                      ? RIDER_EFFECT_NEGATIVE_SIDE_TYPE
                      : 0;
  if((rider->track_segment->surface_flags &
      RIDER_EFFECT_REQUIRED_SURFACE_FLAGS) == 0)
    {
      return RIDER_EFFECT_NOT_FOUND;
    }

  effect_mask = track_effect_type_masks[effect_type];
  forward = (TrackInteractionObject *)
            find_nearby_track_object_in_direction(
    (char *)rider, effect_mask, TRACK_TRAVERSAL_FORWARD,
    RIDER_EFFECT_SEARCH_DISTANCE);
  backward = (TrackInteractionObject *)
             find_nearby_track_object_in_direction(
    (char *)rider, effect_mask, TRACK_TRAVERSAL_REVERSE,
    RIDER_EFFECT_SEARCH_DISTANCE);

  if(backward == 0)
    {
      nearest = forward;
    }
  else if(forward == 0)
    {
      nearest = backward;
    }
  else
    {
      forward_distance = absolute_track_distance(
        forward->position.track_position -
        rider->position.track_position);
      backward_distance = absolute_track_distance(
        backward->position.track_position -
        rider->position.track_position);
      nearest = forward_distance < backward_distance ? forward : backward;
    }

  if(nearest == 0)
    {
      return RIDER_EFFECT_NOT_FOUND;
    }

  distance = absolute_track_distance(nearest->position.track_position -
                                     rider->position.track_position);
  if(distance <= RIDER_EFFECT_CONTACT_DISTANCE)
    {
      return nearest->interaction_selector;
    }
  return RIDER_EFFECT_NOT_FOUND;
}


__inline
static
void
choose_recovery_animation(RiderVisualObject *rider)
{
  RiderAnimationRuntime *animation;

  animation = &rider->animation;
  if(rider->animation_states[RIDER_ANIMATION_MOTION] != 0)
    {
      set_secondary_rider_animation(animation, RIDER_ANIMATION_MOTION);
    }
  else if(rider->animation_states[RIDER_ANIMATION_FAST] != 0)
    {
      set_secondary_rider_animation(animation, RIDER_ANIMATION_FAST);
    }
  else
    {
      set_secondary_rider_animation(animation, RIDER_ANIMATION_STANDING);
    }
}


__inline
static
void
resume_riding(RiderVisualObject *rider)
{
  RiderAnimationRuntime *animation;

  animation = &rider->animation;
  if(rider->animation_states[RIDER_ANIMATION_MOTION] != 0)
    {
      rider->behavior_state = RIDER_BEHAVIOR_RIDING;
      rider->movement_force = RIDER_ACTION_FORCE_LOW;
      set_primary_rider_animation(animation, RIDER_ANIMATION_MOTION);
      set_secondary_rider_animation(animation,
                                    RIDER_ANIMATION_NONE);
    }
  else if(rider->animation_states[RIDER_ANIMATION_FAST] != 0)
    {
      set_secondary_rider_animation(animation,
                                    RIDER_ANIMATION_FAST);
    }
  else
    {
      rider->behavior_state = RIDER_BEHAVIOR_WAITING;
      set_primary_rider_animation(animation, RIDER_ANIMATION_STANDING);
      set_secondary_rider_animation(animation,
                                    RIDER_ANIMATION_NONE);
      rider->movement_force = 0;
    }
}


__inline
static
void
reset_riding(RiderVisualObject *rider)
{
  RiderAnimationRuntime *animation;

  animation = &rider->animation;
  rider->behavior_state = RIDER_BEHAVIOR_WAITING;
  set_primary_rider_animation(animation, RIDER_ANIMATION_STANDING);
  set_secondary_rider_animation(animation,
                                RIDER_ANIMATION_NONE);
  rider->movement_force = 0;
}


__inline
static
void
enter_riding(RiderVisualObject *rider)
{
  RiderAnimationRuntime *animation;

  animation = &rider->animation;
  rider->behavior_state = RIDER_BEHAVIOR_RIDING;
  if(rider->animation_states[RIDER_ANIMATION_MOTION] == 0)
    {
      rider->movement_force = RIDER_ACTION_FORCE_HIGH;
    }
  else
    {
      rider->movement_force = RIDER_ACTION_FORCE_LOW;
    }

  if(rider->animation_ready == 0)
    {
      set_primary_rider_animation(animation, RIDER_ANIMATION_MOTION);
    }
  else if(rider->animation_states[RIDER_ANIMATION_MOTION] == 0)
    {
      set_primary_rider_animation(animation, RIDER_ANIMATION_FAST);
    }
  else
    {
      set_primary_rider_animation(animation, RIDER_ANIMATION_MOTION);
    }
}


static
void
update_waiting_state(RiderVisualObject *rider,
                     int                time_limit,
                     int                direction,
                     int                target,
                     int                target_found)
{
  int lane_offset;
  int clear;

  rider->observed_lane = (signed char)rider_lane_index(rider);

  if(rider->animation_states[RIDER_ANIMATION_MOTION] == 0 &&
     rider->animation_states[RIDER_ANIMATION_FAST] == 0 &&
     rider->animation_ready != 0)
    {
      return;
    }
  if(time_limit <= rider->action_cooldown_tick)
    {
      return;
    }
  if((rand() & RIDER_RANDOM_ODD_MASK) == 0)
    {
      return;
    }

  clear = 0;
  if(target_found != 0)
    {
      clear = target == RIDER_EFFECT_TARGET_SIDE;
    }
  else if(rider_lane_change_is_clear(rider, direction, 0) != 0)
    {
      lane_offset = rider->lane_direction_index ==
                    TRACK_OBJECT_MODE_CROSS_POSITIVE
                          ? 1
                          : -1;
      clear = rider_lane_change_is_clear(rider, direction,
                                         lane_offset) != 0;
    }

  if(clear != 0)
    {
      enter_riding(rider);
    }
}


static
void
update_riding_state(RiderVisualObject *rider,
                    int                time_limit,
                    int                direction,
                    int                target,
                    int                target_found)
{
  int lane;
  unsigned int now;

  if(rider->animation_states[RIDER_ANIMATION_MOTION] != 0)
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_MOTION);
      rider->movement_force = RIDER_ACTION_FORCE_LOW;
    }
  else if(rider->animation_states[RIDER_ANIMATION_FAST] != 0)
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_FAST);
      rider->movement_force = RIDER_ACTION_FORCE_HIGH;
    }
  else if(rider->animation_ready == 0)
    {
      rider->movement_force = RIDER_ACTION_FORCE_LOW;
    }
  else
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_STANDING);
      rider->movement_force = 0;
    }

  if(target_found == 0)
    {
      lane = rider_lane_index(rider);
      if(lane != rider->observed_lane)
        {
          rider->observed_lane = (signed char)rider_lane_index(rider);
          if(rider_lane_change_is_clear(rider, direction, 0) == 0)
            {
              reset_riding(rider);
            }
          return;
        }
    }

  if((rand() & RIDER_RANDOM_ACTION_MASK) != 0 &&
     rider_lane_change_is_clear(
       rider, (direction >> RIDER_DIRECTION_PROBE_SHIFT) +
       RIDER_DIRECTION_PROBE_BASE,
       0) == 0)
    {
      rider->behavior_state = RIDER_BEHAVIOR_RECOVERING;
      if(rider->animation_states[RIDER_ANIMATION_SHAKE] != 0)
        {
          set_primary_rider_animation(&rider->animation,
                                      RIDER_ANIMATION_SHAKE);
        }

      if(rider->animation_states[RIDER_ANIMATION_ATTACK] != 0 &&
         (rider->animation_states[RIDER_ANIMATION_FAST] == 0 ||
          (rand() & RIDER_RANDOM_FAST_MASK) != 0))
        {
          set_secondary_rider_animation(&rider->animation,
                                        RIDER_ANIMATION_ATTACK);
          now = frame_tick;
          if(now - rider->last_sound_tick > RIDER_SOUND_COOLDOWN_TICKS)
            {
              if(queue_family_transient_spatial_sample_event(
                   rider->family_resource_inventory,
                   RIDER_FAMILY_ITEM_TYPE,
                   RIDER_FAMILY_ATTACK_SAMPLE_SLOT,
                   &rider->position.track_position) != 0)
                {
                  rider->last_sound_tick = now;
                }
            }
        }
      else if(rider->animation_states[RIDER_ANIMATION_FAST] != 0)
        {
          set_secondary_rider_animation(&rider->animation,
                                        RIDER_ANIMATION_FAST);
        }
      return;
    }

  if(has_rider_crossed_lane_midpoint(rider) != 0)
    {
      rider->behavior_state = RIDER_BEHAVIOR_WAITING;
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_STANDING);
      rider->movement_force = 0;
      rider->action_cooldown_tick =
        time_limit + RIDER_ACTION_COOLDOWN_TICKS;
      rider->lane_direction_index =
        (rider->lane_direction_index + RIDER_LANE_ROTATION_STEP) &
        RIDER_LANE_ROTATION_MASK;
    }
  else if(target_found != 0 &&
          target != RIDER_EFFECT_TARGET_SIDE &&
          rider->animation_states[RIDER_ANIMATION_FAST] != 0)
    {
      set_secondary_rider_animation(&rider->animation,
                                    RIDER_ANIMATION_FAST);
    }
}


__inline
static
void
update_recovering_state(RiderVisualObject *rider)
{
  unsigned int animation_state;

  animation_state = rider->animation.primary_state;
  if(animation_state == RIDER_ANIMATION_SHAKE)
    {
      return;
    }
  if(animation_state == RIDER_ANIMATION_ATTACK ||
     animation_state == RIDER_ANIMATION_FAST)
    {
      rider->behavior_state = (int)animation_state;
      choose_recovery_animation(rider);
      return;
    }
  resume_riding(rider);
}


__inline
static
void
update_shake_recovery_state(RiderVisualObject *rider)
{
  if(rider->animation.primary_state !=
     RIDER_ANIMATION_ATTACK)
    {
      resume_riding(rider);
    }
}


__inline
static
void
update_fast_recovery_state(RiderVisualObject *rider,
                           int                direction)
{
  rider->movement_force = RIDER_ACTION_FORCE_HIGH;
  if(rider_lane_change_is_clear(rider, direction, 0) != 0)
    {
      resume_riding(rider);
    }
}


void
rider_visual_update(RiderVisualObject *rider)
{
  RacerEntity *player;
  int time_limit;
  int direction;
  int target;
  int target_found;
  int action;

  if(try_acquire_family_resource_render_barrier() != 0)
    {
      refresh_rider_visual_resource_binding(rider);
      release_family_resource_render_barrier();
    }
  player = (RacerEntity *)race_rider_state.player_object;
  if(player == 0)
    {
      time_limit = 0;
    }
  else
    {
      time_limit = player->paired_state->position_z;
    }

  target = find_nearby_track_effect_side(rider);
  target_found = target != RIDER_EFFECT_NOT_FOUND;
  rider->movement_force = 0;

  if(rider->behavior_state == RIDER_BEHAVIOR_CONTACT_LOCKED)
    {
      goto cleanup_animation;
    }

  rider->movement_direction =
    rider_direction_vectors[rider->lane_direction_index];

  direction = 0;
  switch(rider->travel_mode)
    {
    case RIDER_TRAVEL_FIXED_HEADING:
      rider->movement_direction = RIDER_FIXED_MOVEMENT_DIRECTION;
      break;
    case RIDER_TRAVEL_HARD_LEFT:
      direction = RIDER_DIRECTION_HARD_LEFT;
      goto run_state;
    case RIDER_TRAVEL_CENTER:
      direction = RIDER_DIRECTION_CENTER;
      goto run_state;
    case RIDER_TRAVEL_HARD_RIGHT:
      direction = RIDER_DIRECTION_HARD_RIGHT;
run_state:
      switch(rider->behavior_state)
        {
        case RIDER_BEHAVIOR_WAITING:
          update_waiting_state(rider, time_limit, direction, target,
                               target_found);
          break;
        case RIDER_BEHAVIOR_RIDING:
          update_riding_state(rider, time_limit, direction, target,
                              target_found);
          break;
        case RIDER_BEHAVIOR_RECOVERING:
          update_recovering_state(rider);
          break;
        case RIDER_BEHAVIOR_SHAKE_RECOVERY:
          update_shake_recovery_state(rider);
          break;
        case RIDER_BEHAVIOR_FAST_RECOVERY:
          update_fast_recovery_state(rider, direction);
          break;
        }
      break;
    case RIDER_TRAVEL_FLAG:
      if(rider->animation_ready != 0 &&
         rider->behavior_state != RIDER_BEHAVIOR_FLAG)
        {
          rider->behavior_state = RIDER_BEHAVIOR_FLAG;
          set_primary_rider_animation(&rider->animation,
                                      RIDER_ANIMATION_FLAG);
        }
      break;
    }

  rider->lane_probe_pending = 0;
  action = rider->movement_force * RIDER_ACTION_FORCE_SCALE;
  apply_race_object_heading_and_force(
    (RacerEntity *)(void *)rider,
    rider->movement_direction,
    action, action);

cleanup_animation:
  if(rider->animation_ready != 0 &&
     *(const unsigned int *)rider->animation.animation_resource ==
     RIDER_ANIMATION_RESOURCE_TAG)
    {
      advance_car_animation(&rider->animation);
    }
}
