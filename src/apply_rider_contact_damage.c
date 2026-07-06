#include "division_helpers.h"
#include "rider_contact_runtime.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_queue.h"

enum RiderContactDamageValue
{
  RIDER_CONTACT_FIXED_SHIFT = 8,
  RIDER_CONTACT_FIXED_ROUNDING_BIAS = 0xFF,
  RIDER_CONTACT_SIGN_SHIFT = 31,
  RIDER_CONTACT_HALF_SHIFT = 1,
  RIDER_CONTACT_QUARTER_SHIFT = 2,
  RIDER_CONTACT_QUARTER_ROUNDING_BIAS = 3,
  RIDER_CONTACT_AUDIO_UNARMED = 0x0E,
  RIDER_CONTACT_AUDIO_CLUB = 0x0B,
  RIDER_CONTACT_AUDIO_CHAIN = 0x0A,
  RIDER_CONTACT_AUDIO_KICK_FIRST = 0x0C,
  RIDER_CONTACT_AUDIO_KICK_VARIANTS = 2,
  RIDER_CONTACT_RECOVERY_PRIMARY = 4,
  RIDER_CONTACT_RECOVERY_ALTERNATE = 5
};

static
int
divide_rider_contact_by_two_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RIDER_CONTACT_SIGN_SHIFT)) >>
         RIDER_CONTACT_HALF_SHIFT;
}


static
int
divide_rider_contact_by_four_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_CONTACT_QUARTER_ROUNDING_BIAS;
    }
  return value >> RIDER_CONTACT_QUARTER_SHIFT;
}


static
int
divide_rider_contact_fixed_toward_zero(int value)
{
  if(value < 0)
    {
      value += RIDER_CONTACT_FIXED_ROUNDING_BIAS;
    }
  return value >> RIDER_CONTACT_FIXED_SHIFT;
}


int32
apply_rider_contact_damage(RacerEntity *rider,
                           RacerEntity *target)
{
  RacerEntity *state;
  RiderRuntimeSlot *runtime;
  SignedDivisionResult health_ratio;
  RiderAttackStyle attack_style;
  int damage;
  int scaled_damage;
  int minimum_damage;
  int bike_damage;
  int audio_event;

  state = rider->paired_state;
  runtime = state->runtime;

  health_ratio = signed_divide_with_remainder(
    (unsigned int)runtime->maximum_health,
    (unsigned int)(runtime->recovery_health_ceiling <<
                   RIDER_CONTACT_FIXED_SHIFT));
  damage = (runtime->rank_weight * health_ratio.quotient) >>
           RIDER_CONTACT_FIXED_SHIFT;
  minimum_damage = divide_rider_contact_by_two_toward_zero(
    runtime->rank_weight);
  if(minimum_damage > damage)
    {
      damage = minimum_damage;
    }

  attack_style = (RiderAttackStyle)state->attack_style;
  if((unsigned int)attack_style >= RIDER_ATTACK_STYLE_COUNT)
    {
      return 0;
    }
  scaled_damage = divide_rider_contact_fixed_toward_zero(
    damage * rider_contact_scale_by_mode[attack_style]);

  if(rider == (RacerEntity *)race_rider_state.player_object ||
     target == (RacerEntity *)race_rider_state.player_object)
    {
      switch(attack_style)
        {
        case RIDER_ATTACK_STYLE_PUNCH:
          audio_event = RIDER_CONTACT_AUDIO_UNARMED;
          break;
        case RIDER_ATTACK_STYLE_CLUB:
          audio_event = RIDER_CONTACT_AUDIO_CLUB;
          break;
        case RIDER_ATTACK_STYLE_CHAIN:
          audio_event = RIDER_CONTACT_AUDIO_CHAIN;
          break;
        case RIDER_ATTACK_STYLE_KICK:
          audio_event = RIDER_CONTACT_AUDIO_KICK_FIRST +
                        (frame_tick & (RIDER_CONTACT_AUDIO_KICK_VARIANTS - 1));
          break;
        default:
          audio_event = 0;
          break;
        }
      if(audio_event != 0)
        {
          queue_spatial_audio_event(
            audio_event, &rider->position_z, &rider->velocity.forward);
        }
    }

  if(state->attack_style == RIDER_ATTACK_STYLE_KICK)
    {
      bike_damage = divide_rider_contact_by_four_toward_zero(
        scaled_damage) + 1;
    }
  else
    {
      bike_damage = 0;
    }

  return apply_rider_collision_damage(
    target, scaled_damage, bike_damage,
    bike_damage == 0 ? RIDER_CONTACT_RECOVERY_PRIMARY
                         : RIDER_CONTACT_RECOVERY_ALTERNATE);
}
