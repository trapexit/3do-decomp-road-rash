#include "audio.h"
#include "stdlib.h"

#include "racer_runtime.h"
#include "spatial_audio_event_queue.h"

typedef struct UnsignedDivisionResult
{
  unsigned int quotient;
  unsigned int remainder;
} UnsignedDivisionResult;

enum
{
  SIGN_BIT_SHIFT = 31,
  DIVIDE_BY_TWO_SHIFT = 1,
  DIVIDE_BY_FOUR_SHIFT = 2,
  DIVIDE_BY_FOUR_ROUNDING_BIAS = 3,
  DIVIDE_BY_SIXTEEN_SHIFT = 4,
  DIVIDE_BY_SIXTEEN_ROUNDING_BIAS = 15,
  FIXED_8_8_SHIFT = 8,
  FIXED_8_8_ROUNDING_BIAS = 0xFF,
  CONTACT_IMPULSE_MAGNITUDE_SHIFT = 10,
  CONTACT_PRODUCT_SHIFT = 18,
  CONTACT_PRODUCT_ROUNDING_BIAS = 0x3FFFF,

  RIDER_MODE_NORMAL = 0,
  RIDER_MODE_RECOVERY = 1,
  RIDER_MODE_STOPPED = 2,
  RIDER_STATUS_RECOVERY_MASK = 3,
  RIDER_NORMAL_BASE_DRAG = 8,

  RIDER_ANIMATION_IDLE = 0,
  RIDER_ANIMATION_RECOVERY_A = 0x1C,
  RIDER_ANIMATION_RECOVERY_B = 0x1D,
  RIDER_ANIMATION_RECOVERY_TRIGGER = 0x21,
  RIDER_ANIMATION_RECOVERY_TRANSITION = 0x22,
  RIDER_SECONDARY_ANIMATION_RECOVERY_IMPACT = 0x26,

  CONTACT_CLASS_HEIGHT_REDUCIBLE = 1,
  CONTACT_CLASS_TRACK_HAZARD = 2,
  CONTACT_CLASS_DAMAGE_SPLIT_MASK = 0x0C,
  CONTACT_CLASS_VERTICAL_POSITION_MASK = 0x15,
  CONTACT_CLASS_LOW_SPEED_GATE = 0x20,
  CONTACT_FLAG_VERTICAL_CONTACT = 0x20,
  CONTACT_FLAG_VERTICAL_OVERLAP = 0x40,

  RIDER_CONTACT_STATIONARY_SPEED = 10,
  RIDER_CONTACT_HEIGHT_LIMIT = 0x5000,
  RIDER_STATE_VERTICAL_RESPONSE_SPEED = 500,
  RIDER_STATE_VERTICAL_SPEED_LIMIT = 0x150,
  RIDER_RECOVERY_RESTART_SPEED = 1700,
  RIDER_MEDIUM_IMPACT_SPEED = 2000,
  RIDER_LATERAL_SEPARATION_SPEED_LIMIT = 3000,
  RIDER_HARD_IMPACT_SPEED = 6000,
  RIDER_MAX_FORWARD_SPEED = 30000,
  RIDER_MAX_VERTICAL_SPEED = 10000,
  RIDER_MIN_VERTICAL_SPEED = -10000,
  RIDER_STATE_MIN_FORWARD_SPEED = -2000,
  RIDER_OBJECT_MIN_FORWARD_SPEED = -1000,
  RIDER_OBJECT_VERTICAL_SPEED_LIMIT = 0x600,

  RIDER_AUDIO_HARD_RECOVERY_BASE = 6,
  RIDER_AUDIO_MEDIUM_RECOVERY_BASE = 5,
  RIDER_AUDIO_HARD_IMPACT_BASE = 3,
  RIDER_AUDIO_MEDIUM_IMPACT_BASE = 2,
  RIDER_AUDIO_VARIANT_MASK = 1,

  CONTACT_TRACK_JITTER_DIVISOR = 0x21,
  CONTACT_TRACK_JITTER_CENTER = 0x10,
  CONTACT_HEIGHT_MULTIPLIER = 2,
  CONTACT_STEERING_MULTIPLIER = 2,
  CONTACT_VERTICAL_RESPONSE_MULTIPLIER = 4,

  TRACK_CONTACT_SHORT_A = 3,
  TRACK_CONTACT_SHORT_B = 6,
  TRACK_CONTACT_SHORT_C = 9,
  TRACK_CONTACT_MEDIUM_A = 4,
  TRACK_CONTACT_MEDIUM_B = 7,
  TRACK_CONTACT_MEDIUM_C = 10,
  TRACK_CONTACT_LONG_A = 5,
  TRACK_CONTACT_LONG_B = 8,
  TRACK_CONTACT_LONG_C = 11,
  TRACK_CONTACT_SHORT_TICKS = 0x3C,
  TRACK_CONTACT_MEDIUM_TICKS = 0x78,
  TRACK_CONTACT_LONG_TICKS = 0xB4,

  RIDER_REACTION_NONE = 0,
  RIDER_REACTION_STANDARD = 1,
  RIDER_REACTION_SEVERE = 2,
  RIDER_REACTION_GLANCING = 3,
  RIDER_REACTION_HEIGHT_THRESHOLD = 0x2000,
  RIDER_REACTION_SEVERE_HEIGHT = 0x2800,
  RIDER_LOW_FORWARD_SPEED = 300,
  RIDER_TRACK_CONTACT_ANIMATION_SPEED = 1000,
  RIDER_LATERAL_IMPACT_SOUND_THRESHOLD = 1000,
  RIDER_GENERIC_CONTACT_SPEED = 500,
  RIDER_TRACK_CONTACT_SPEED = 2000,
  CONTACT_LOW_SPEED_GATE_EXCLUDED_VALUE = 2,
  RIDER_RELATIVE_IMPACT_SPEED = 6000,
  RIDER_RECOVERY_VERTICAL_SPEED_DELTA = 0x150,
  RIDER_STATE_RECOVERY_VERTICAL_DELTA = 0x200,
  RIDER_OBJECT_RECOVERY_VERTICAL_DELTA = 0x100
};

__value_in_regs UnsignedDivisionResult
unsigned_divide_with_remainder(unsigned int divisor,
                               unsigned int numerator);
void
begin_rider_recovery(RacerEntity         *rider,
                     const RacerVelocity *state_velocity_delta,
                     const RacerVelocity *object_velocity_delta,
                     int                  reaction_kind);
int
apply_rider_collision_damage(RacerEntity *rider,
                             int          rider_damage,
                             int          bike_damage,
                             int          reaction_kind);
int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);
int
select_secondary_rider_animation(RiderAnimationRuntime *animation,
                                 int                    state);
void
advance_racer_track_position(RacerEntity *rider,
                             int          track_delta,
                             int          update_collisions);

static
int
absolute_magnitude(int value)
{
  if(value <= 0)
    {
      value = -value;
    }
  return value;
}


static
int
divide_by_two_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> SIGN_BIT_SHIFT)) >>
         DIVIDE_BY_TWO_SHIFT;
}


static
int
divide_by_four_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_FOUR_ROUNDING_BIAS;
    }
  return value >> DIVIDE_BY_FOUR_SHIFT;
}


static
int
divide_by_sixteen_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_SIXTEEN_ROUNDING_BIAS;
    }
  return value >> DIVIDE_BY_SIXTEEN_SHIFT;
}


static
int
scale_fixed_8_8_toward_zero(int value)
{
  if(value < 0)
    {
      value += FIXED_8_8_ROUNDING_BIAS;
    }
  return value >> FIXED_8_8_SHIFT;
}


static
void
queue_random_contact_audio(int          event_base,
                           RacerEntity *rider)
{
  int event_type;

  event_type =
    (int)(GetAudioTime() & RIDER_AUDIO_VARIANT_MASK) + event_base;
  queue_spatial_audio_event(event_type, &rider->position_z,
                            &rider->velocity.forward);
}


static
void
separate_rider_laterally(RacerEntity       *rider,
                         const RacerEntity *contact,
                         const RacerEntity *step_source)
{
  int separation_step;
  int lateral_position;

  lateral_position = rider->position_x;
  separation_step = divide_by_sixteen_toward_zero(
    contact->collision_half_width_x + rider->collision_half_width_x);
  if(lateral_position > contact->position_x)
    {
      lateral_position += step_source->time_step * separation_step;
    }
  else
    {
      lateral_position -= step_source->time_step * separation_step;
    }
  rider->position_x = lateral_position;
}


static
void
clamp_rider_contact_velocity(RacerEntity *rider,
                             int          minimum_forward_speed)
{
  int value;

  value = rider->velocity.forward;
  if(value > RIDER_MAX_FORWARD_SPEED)
    {
      value = RIDER_MAX_FORWARD_SPEED;
    }
  else if(value < minimum_forward_speed)
    {
      value = minimum_forward_speed;
    }
  rider->velocity.forward = value;

  value = rider->velocity.vertical;
  if(value > RIDER_MAX_VERTICAL_SPEED)
    {
      value = RIDER_MAX_VERTICAL_SPEED;
    }
  else if(value < RIDER_MIN_VERTICAL_SPEED)
    {
      value = RIDER_MIN_VERTICAL_SPEED;
    }
  rider->velocity.vertical = value;
}


void
reset_rider_to_normal_mode(RacerEntity *rider)
{
  RacerEntity *state;
  RiderRuntimeSlot *runtime;

  rider->rider_mode = RIDER_MODE_NORMAL;
  state = rider->paired_state;
  select_primary_rider_animation(&rider->animation,
                                 RIDER_ANIMATION_IDLE);
  select_primary_rider_animation(&state->animation,
                                 RIDER_ANIMATION_IDLE);
  select_secondary_rider_animation(&rider->animation,
                                   RIDER_ANIMATION_IDLE);
  select_secondary_rider_animation(&state->animation,
                                   RIDER_ANIMATION_IDLE);

  rider->base_drag = RIDER_NORMAL_BASE_DRAG;
  rider->animation.current_frame = 0;
  runtime = state->runtime;
  runtime->current_health = runtime->recovery_health_ceiling;
  state->status_flags = (unsigned char)(
    state->status_flags & (unsigned char)~RIDER_STATUS_RECOVERY_MASK);
  rider->acceleration_disabled = 0;
  rider->drive_acceleration = 0;
  rider->steering_force = 0;
  rider->velocity.lateral = 0;
  rider->steering_angle = 0;

  rider->collision_half_width_x =
    upright_motion_defaults.collision_half_width_x;
  rider->collision_half_length_z =
    upright_motion_defaults.collision_half_length_z;
  rider->collision_half_height_y =
    upright_motion_defaults.collision_half_height_y;

  sync_rider_state_from_object(rider);
}


void
racer_state_contact_response(RacerEntity *state)
{
  UnsignedDivisionResult random_offset;
  RacerEntity *contact;
  RacerEntity *rider;
  RiderAnimationRuntime *animation;
  int forward_speed;
  int forward_speed_magnitude;
  int vertical_speed_magnitude;

  contact = state->contact;
  rider = state->owner;
  animation = &state->animation;

  if(contact->collision_class != CONTACT_CLASS_TRACK_HAZARD &&
     (contact != rider ||
      contact->velocity.forward >= RIDER_CONTACT_STATIONARY_SPEED ||
      contact->position_y != 0))
    {
      if(rider->rider_mode != RIDER_MODE_STOPPED ||
         contact->velocity.forward >= RIDER_RECOVERY_RESTART_SPEED)
        {
          if((contact->collision_class &
              CONTACT_CLASS_VERTICAL_POSITION_MASK) != 0)
            {
              state->position_y = contact->collision_half_height_y *
                                  CONTACT_HEIGHT_MULTIPLIER;
            }
          if(state->position_y > RIDER_CONTACT_HEIGHT_LIMIT)
            {
              state->position_y = RIDER_CONTACT_HEIGHT_LIMIT;
            }

          forward_speed = state->velocity.forward;
          forward_speed_magnitude = absolute_magnitude(forward_speed);
          if(forward_speed_magnitude >
             RIDER_STATE_VERTICAL_RESPONSE_SPEED)
            {
              vertical_speed_magnitude = absolute_magnitude(
                divide_by_two_toward_zero(state->velocity.vertical));
              state->velocity.vertical =
                vertical_speed_magnitude +
                (absolute_magnitude(forward_speed) >>
                 DIVIDE_BY_SIXTEEN_SHIFT);
            }
          else
            {
              state->velocity.vertical = 0;
            }

          state->velocity.forward =
            forward_speed - divide_by_four_toward_zero(forward_speed);
          if(animation->primary_state !=
             RIDER_ANIMATION_RECOVERY_TRIGGER &&
             animation->primary_state != RIDER_ANIMATION_RECOVERY_B)
            {
              select_primary_rider_animation(
                animation, RIDER_ANIMATION_RECOVERY_A);
              select_secondary_rider_animation(
                animation, RIDER_ANIMATION_RECOVERY_A);
            }
          if(state->velocity.vertical >
             RIDER_STATE_VERTICAL_SPEED_LIMIT)
            {
              state->velocity.vertical = RIDER_STATE_VERTICAL_SPEED_LIMIT;
            }

          if(rider->rider_mode == RIDER_MODE_STOPPED &&
             contact->velocity.forward > RIDER_RECOVERY_RESTART_SPEED)
            {
              state->velocity.forward += divide_by_four_toward_zero(
                contact->velocity.forward);
              rider->rider_mode = RIDER_MODE_RECOVERY;
              select_primary_rider_animation(
                animation, RIDER_ANIMATION_RECOVERY_TRANSITION);
              select_secondary_rider_animation(
                animation, RIDER_SECONDARY_ANIMATION_RECOVERY_IMPACT);
            }

          if(rider == (RacerEntity *)race_rider_state.player_object)
            {
              forward_speed = state->velocity.forward;
              if(forward_speed > RIDER_HARD_IMPACT_SPEED)
                {
                  queue_random_contact_audio(
                    RIDER_AUDIO_HARD_RECOVERY_BASE, state);
                }
              else if(forward_speed > RIDER_MEDIUM_IMPACT_SPEED)
                {
                  queue_random_contact_audio(
                    RIDER_AUDIO_MEDIUM_RECOVERY_BASE, state);
                }
            }
        }

      if(state->velocity.vertical <
         RIDER_LATERAL_SEPARATION_SPEED_LIMIT)
        {
          separate_rider_laterally(state, contact, rider);
          random_offset = unsigned_divide_with_remainder(
            CONTACT_TRACK_JITTER_DIVISOR, (unsigned int)rand());
          advance_racer_track_position(
            state,
            (int)random_offset.remainder - CONTACT_TRACK_JITTER_CENTER,
            1);
        }
    }

  state->contact_impulse.lateral = 0;
  state->contact_impulse.vertical = 0;
  state->contact_impulse.forward = 0;
  state->collision_flags = 0;
  clamp_rider_contact_velocity(state, RIDER_STATE_MIN_FORWARD_SPEED);
}


void
racer_object_contact_response(RacerEntity *rider)
{
  RacerEntity *state;
  RacerEntity *contact;
  TrackContactDescriptor *track_contact;
  RacerVelocity *contact_impulse;
  RacerVelocity state_velocity_delta;
  RacerVelocity object_velocity_delta;
  int impulse_scale;
  int impulse_magnitude;
  int rider_mode;
  int reaction_kind;
  int recovery_started;
  int previous_lateral_velocity;
  int value;
  int relative_forward_speed;
  int steering_limit;

  state = rider->paired_state;
  impulse_scale = rider->collision_impulse_scale_8_8;
  contact = rider->contact;
  contact_impulse = &rider->contact_impulse;
  impulse_magnitude =
    (absolute_magnitude(contact_impulse->lateral) +
     absolute_magnitude(contact_impulse->vertical) +
     absolute_magnitude(contact_impulse->forward)) >>
    CONTACT_IMPULSE_MAGNITUDE_SHIFT;
  recovery_started = 0;
  previous_lateral_velocity = rider->velocity.lateral;

  if(contact == state &&
     rider->velocity.forward < RIDER_CONTACT_STATIONARY_SPEED &&
     rider->position_y == 0)
    {
      goto cleanup;
    }

  rider_mode = rider->rider_mode;
  if(rider_mode != RIDER_MODE_NORMAL)
    {
      rider->velocity.vertical += RIDER_RECOVERY_VERTICAL_SPEED_DELTA;
      goto separate_laterally;
    }

  if(contact->collision_class == CONTACT_CLASS_TRACK_HAZARD)
    {
      track_contact = contact->track_contact;
      if(state->last_contact_track_cell !=
         (rider->position_z >> FIXED_8_8_SHIFT))
        {
          switch(track_contact->response_kind)
            {
            case TRACK_CONTACT_SHORT_A:
            case TRACK_CONTACT_SHORT_B:
            case TRACK_CONTACT_SHORT_C:
              rider->slide_timer = TRACK_CONTACT_SHORT_TICKS;
              break;
            case TRACK_CONTACT_MEDIUM_A:
            case TRACK_CONTACT_MEDIUM_B:
            case TRACK_CONTACT_MEDIUM_C:
              rider->slide_timer = TRACK_CONTACT_MEDIUM_TICKS;
              break;
            case TRACK_CONTACT_LONG_A:
            case TRACK_CONTACT_LONG_B:
            case TRACK_CONTACT_LONG_C:
              rider->slide_timer = TRACK_CONTACT_LONG_TICKS;
              break;
            }

          if(rider->velocity.forward >
             RIDER_TRACK_CONTACT_ANIMATION_SPEED)
            {
              state->pending_contact_animation =
                track_contact->animation_state;
              state->pending_contact_animation_frame = 1;
              state->contact_frame_tick = (int)frame_tick;
              state->last_contact_track_cell =
                rider->position_z >> FIXED_8_8_SHIFT;
              rider->animation.primary_frame = 1;
              rider->animation.primary_state =
                state->pending_contact_animation;
            }
          rider->velocity.lateral = 0;
          rider->steering_angle += divide_by_two_toward_zero(
            rider->steering_angle);
        }
      goto cleanup;
    }

  state->pending_contact_animation_frame = 0;
  if(contact->collision_class == CONTACT_CLASS_HEIGHT_REDUCIBLE)
    {
      contact->collision_half_height_y = divide_by_two_toward_zero(
        contact->collision_half_height_y);
    }

  if(rider == (RacerEntity *)race_rider_state.player_object)
    {
      value = absolute_magnitude(contact_impulse->forward);
      if(value > RIDER_HARD_IMPACT_SPEED)
        {
          if(rider->rider_mode == RIDER_MODE_NORMAL)
            {
              queue_random_contact_audio(RIDER_AUDIO_HARD_RECOVERY_BASE,
                                         rider);
            }
          queue_random_contact_audio(RIDER_AUDIO_HARD_IMPACT_BASE, rider);
        }
      else if(value > RIDER_MEDIUM_IMPACT_SPEED ||
              absolute_magnitude(contact_impulse->lateral) >
              RIDER_LATERAL_IMPACT_SOUND_THRESHOLD)
        {
          if(rider->rider_mode == RIDER_MODE_NORMAL)
            {
              queue_random_contact_audio(
                RIDER_AUDIO_MEDIUM_RECOVERY_BASE, rider);
            }
          queue_random_contact_audio(RIDER_AUDIO_MEDIUM_IMPACT_BASE,
                                     rider);
        }
    }

  value = contact->collision_half_height_y;
  if(value < RIDER_REACTION_HEIGHT_THRESHOLD)
    {
      reaction_kind = RIDER_REACTION_NONE;
    }
  else if((rider->collision_flags &
           CONTACT_FLAG_VERTICAL_OVERLAP) != 0)
    {
      reaction_kind = value < RIDER_REACTION_SEVERE_HEIGHT
                            ? RIDER_REACTION_STANDARD
                            : RIDER_REACTION_SEVERE;
    }
  else
    {
      reaction_kind = RIDER_REACTION_GLANCING;
    }

  contact_impulse->lateral = scale_fixed_8_8_toward_zero(
    impulse_scale * contact_impulse->lateral);
  contact_impulse->vertical = scale_fixed_8_8_toward_zero(
    impulse_scale * contact_impulse->vertical);
  contact_impulse->forward = scale_fixed_8_8_toward_zero(
    impulse_scale * contact_impulse->forward);

  if(reaction_kind == RIDER_REACTION_NONE)
    {
      value = rider->velocity.forward;
      if(value < RIDER_LOW_FORWARD_SPEED &&
         rider->collision_flags == CONTACT_FLAG_VERTICAL_CONTACT)
        {
          advance_racer_track_position(
            rider, rider->time_step * CONTACT_HEIGHT_MULTIPLIER, 0);
          goto separate_laterally;
        }

      if(contact->collision_class == CONTACT_CLASS_LOW_SPEED_GATE &&
         contact->last_contact_track_cell !=
         CONTACT_LOW_SPEED_GATE_EXCLUDED_VALUE)
        {
          if(value < RIDER_TRACK_CONTACT_SPEED)
            {
              goto separate_laterally;
            }
        }
      else if(value < RIDER_GENERIC_CONTACT_SPEED)
        {
          goto separate_laterally;
        }

      rider->position_y += divide_by_two_toward_zero(
        contact->collision_half_height_y);
      value = contact->collision_half_height_y *
              absolute_magnitude(contact_impulse->forward) *
              CONTACT_HEIGHT_MULTIPLIER;
      if(value < 0)
        {
          value += CONTACT_PRODUCT_ROUNDING_BIAS;
        }
      rider->velocity.vertical +=
        (value >> CONTACT_PRODUCT_SHIFT) *
        CONTACT_VERTICAL_RESPONSE_MULTIPLIER;
      goto after_lateral_separation;
    }

  if((contact->collision_class & CONTACT_CLASS_DAMAGE_SPLIT_MASK) != 0)
    {
      recovery_started = apply_rider_collision_damage(
        rider, 0,
        divide_by_sixteen_toward_zero(impulse_magnitude),
        reaction_kind);
    }
  else
    {
      recovery_started = apply_rider_collision_damage(
        rider, impulse_magnitude, impulse_magnitude, reaction_kind);
    }

  if(reaction_kind == RIDER_REACTION_STANDARD ||
     reaction_kind == RIDER_REACTION_SEVERE)
    {
      if(recovery_started == 0)
        {
          relative_forward_speed = rider->velocity.forward -
                                   contact->world_velocity.forward;
          if(relative_forward_speed <= RIDER_RELATIVE_IMPACT_SPEED)
            {
              rider->velocity.forward += contact_impulse->forward;
            }
          else
            {
              state_velocity_delta.forward = 0;
              state_velocity_delta.lateral = 0;
              state_velocity_delta.vertical =
                RIDER_STATE_RECOVERY_VERTICAL_DELTA;
              object_velocity_delta.forward = 0;
              object_velocity_delta.lateral = 0;
              object_velocity_delta.vertical =
                RIDER_OBJECT_RECOVERY_VERTICAL_DELTA;
              begin_rider_recovery(rider, &state_velocity_delta,
                                   &object_velocity_delta, reaction_kind);
            }
          goto separate_laterally;
        }
      if(reaction_kind == RIDER_REACTION_SEVERE)
        {
          rider->velocity.forward += contact_impulse->forward;
          goto separate_laterally;
        }
      goto separate_laterally;
    }

  if(recovery_started != 0)
    {
      rider->velocity.vertical += scale_fixed_8_8_toward_zero(
        absolute_magnitude(contact_impulse->forward));
      goto separate_laterally;
    }

  value = absolute_magnitude(contact_impulse->forward);
  if(rider->position_x > contact->position_x)
    {
      if(rider->velocity.lateral < 0)
        {
          rider->velocity.lateral = 0;
        }
      rider->velocity.lateral += divide_by_two_toward_zero(value);
    }
  else
    {
      if(rider->velocity.lateral > 0)
        {
          rider->velocity.lateral = 0;
        }
      rider->velocity.lateral -= divide_by_two_toward_zero(value);
    }
  rider->velocity.forward -= divide_by_four_toward_zero(value);
  if(rider->velocity.forward < 0)
    {
      rider->velocity.forward = 0;
    }

separate_laterally:
  separate_rider_laterally(rider, contact, rider);

after_lateral_separation:
  if(recovery_started != 0)
    {
      goto cleanup;
    }
  if(rider->velocity.lateral != previous_lateral_velocity)
    {
      value = rider->collision_steering_gain_8_8 *
              (rider->velocity.lateral - previous_lateral_velocity) *
              CONTACT_STEERING_MULTIPLIER;
      value = scale_fixed_8_8_toward_zero(value);
      value += rider->steering_angle;
      rider->steering_angle = value;
      steering_limit = rider->steering_limit;
      if(value > steering_limit)
        {
          rider->steering_angle = steering_limit;
        }
      else if(value < -steering_limit)
        {
          rider->steering_angle = -steering_limit;
        }
    }

cleanup:
  if(rider->velocity.vertical > RIDER_OBJECT_VERTICAL_SPEED_LIMIT)
    {
      rider->velocity.vertical = RIDER_OBJECT_VERTICAL_SPEED_LIMIT;
    }
  contact_impulse->lateral = 0;
  contact_impulse->vertical = 0;
  contact_impulse->forward = 0;
  rider->collision_flags = 0;
  clamp_rider_contact_velocity(rider, RIDER_OBJECT_MIN_FORWARD_SPEED);
}
