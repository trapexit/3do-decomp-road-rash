#include "racer_runtime.h"
#include "spatial_audio_event_queue.h"

enum
{
  CHALLENGE_RECOVERY_AUDIO_EVENT = 0x16,
  RIDER_MODE_NORMAL = 0,
  RIDER_MODE_RECOVERY = 1,
  RIDER_RECOVERY_STATE_FLAGS = 3,
  RIDER_RECOVERY_BASE_DRAG = 0x5F,
  RIDER_STATE_RECOVERY_BASE_DRAG = 0x46,
  RIDER_RECOVERY_SEPARATION_HEIGHT = 0x4B00,
  RIDER_RECOVERY_KIND_SEVERE = 2,
  RIDER_RECOVERY_KIND_ALTERNATE_ANIMATION_A = 4,
  RIDER_RECOVERY_KIND_ALTERNATE_ANIMATION_B = 5,
  RIDER_ANIMATION_RECOVERY_B = 0x1D,
  RIDER_ANIMATION_RECOVERY_TRIGGER = 0x21,
  RIDER_ANIMATION_RECOVERY_ALTERNATE = 0x23,
  RIDER_SECONDARY_ANIMATION_DISABLED = -1
};

int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);
int
select_secondary_rider_animation(RiderAnimationRuntime *animation,
                                 int                    state);

void
begin_rider_recovery(RacerEntity         *rider,
                     const RacerVelocity *state_velocity_delta,
                     const RacerVelocity *rider_velocity_delta,
                     int                  recovery_kind)
{
  RacerEntity *state;
  RacerEntity *challenge;
  RiderAnimationRuntime *animation;

  state = rider->paired_state;
  animation = &state->animation;

  if(rider->rider_mode != RIDER_MODE_NORMAL)
    {
      return;
    }

  challenge = (RacerEntity *)race_rider_state.challenge_object;
  if(challenge != 0 && state == challenge->paired_state)
    {
      queue_spatial_audio_event(CHALLENGE_RECOVERY_AUDIO_EVENT, 0, 0);
    }

  state->position_z = rider->position_z;
  state->position_y = rider->position_y;
  state->position_aux = rider->position_aux;
  state->position_x = rider->position_x;
  state->track_segment = rider->track_segment;
  state->previous_position_z = rider->previous_position_z;
  state->previous_position_y = rider->previous_position_y;
  state->previous_position_aux = rider->previous_position_aux;
  state->previous_position_x = rider->previous_position_x;
  state->previous_track_segment = rider->previous_track_segment;

  state->orientation.base_heading = rider->orientation.base_heading;
  state->orientation.movement_heading =
    rider->orientation.movement_heading;
  state->orientation.steering_heading =
    rider->orientation.steering_heading;
  state->orientation.steering_heading = 0;

  rider->base_drag = RIDER_RECOVERY_BASE_DRAG;
  state->base_drag = RIDER_STATE_RECOVERY_BASE_DRAG;

  state->velocity.forward = rider->velocity.forward;
  state->velocity.lateral = rider->velocity.lateral;
  state->velocity.vertical = rider->velocity.vertical;
  state->lateral_velocity_sample = rider->lateral_velocity_sample;
  state->world_velocity.forward = rider->world_velocity.forward;
  state->world_velocity.lateral = rider->world_velocity.lateral;
  state->world_velocity.vertical = rider->world_velocity.vertical;

  state->steering_force = 0;
  state->surface_acceleration_min = 0;
  state->drive_acceleration = 0;
  state->surface_acceleration_max = 0;
  state->track_contact = 0;
  state->heading = 0;
  state->slip_amount = 0;
  state->collision_scratch = 0;
  state->collision_flags = 0;
  state->contact = 0;

  state->left_surface_bound = rider->left_surface_bound;
  state->right_surface_bound = rider->right_surface_bound;
  state->left_collision_bound = rider->left_collision_bound;
  state->right_collision_bound = rider->right_collision_bound;
  state->secondary_track = rider->secondary_track;
  state->primary_track = rider->primary_track;
  state->surface_drag = rider->surface_drag;
  state->surface_contact_scale_8_8 = rider->surface_contact_scale_8_8;

  if(state_velocity_delta != 0)
    {
      state->velocity.lateral += state_velocity_delta->lateral;
      state->velocity.vertical += state_velocity_delta->vertical;
      state->velocity.forward += state_velocity_delta->forward;
    }

  if(rider_velocity_delta != 0)
    {
      rider->velocity.lateral += rider_velocity_delta->lateral;
      rider->velocity.vertical += rider_velocity_delta->vertical;
      rider->velocity.forward += rider_velocity_delta->forward;
    }

  state->position_y += RIDER_RECOVERY_SEPARATION_HEIGHT;
  rider->position_y += rider->contact_threshold;
  rider->rider_mode = RIDER_MODE_RECOVERY;
  rider->recovery_kind = recovery_kind;
  state->status_flags =
    (unsigned char)(state->status_flags | RIDER_RECOVERY_STATE_FLAGS);
  rider->acceleration_disabled = 1;
  rider->render_position_z = 0;
  rider->steering_angle = 0;

  rider->collision_half_width_x =
    impact_motion_defaults.collision_half_width_x;
  rider->collision_half_length_z =
    impact_motion_defaults.collision_half_length_z;
  rider->collision_half_height_y =
    impact_motion_defaults.collision_half_height_y;

  switch(recovery_kind)
    {
    case RIDER_RECOVERY_KIND_SEVERE:
      select_primary_rider_animation(animation,
                                     RIDER_ANIMATION_RECOVERY_B);
      break;
    case RIDER_RECOVERY_KIND_ALTERNATE_ANIMATION_A:
    case RIDER_RECOVERY_KIND_ALTERNATE_ANIMATION_B:
      select_primary_rider_animation(
        animation, RIDER_ANIMATION_RECOVERY_ALTERNATE);
      break;
    default:
      select_primary_rider_animation(
        animation, RIDER_ANIMATION_RECOVERY_TRIGGER);
      break;
    }

  select_secondary_rider_animation(
    animation, RIDER_SECONDARY_ANIMATION_DISABLED);
}
