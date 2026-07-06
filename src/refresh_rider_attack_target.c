#include "stdlib.h"

#include "division_helpers.h"
#include "rider_contact_runtime.h"
#include "track_object_query.h"

enum RiderAttackTargetSearchConstant
{
  RIDER_ATTACK_PRIMARY_TARGET_KIND = 8,
  RIDER_ATTACK_FALLBACK_TARGET_KIND = 0x38,
  RIDER_ATTACK_TARGET_SEARCH_RANGE = 0x1E0
};

void
rider_update_mode_4(int rider);
int
apply_ai_rider_navigation_control(RacerEntity *rider);

RacerEntity *
refresh_rider_attack_target(RacerEntity *rider)
{
  RacerEntity *state;
  RacerEntity *candidate;

  state = rider->paired_state;
  candidate = (RacerEntity *)find_nearest_longitudinal_track_object(
    (TrackInteractionObject *)state,
    RIDER_ATTACK_PRIMARY_TARGET_KIND,
    RIDER_ATTACK_TARGET_SEARCH_RANGE);
  if(candidate == 0)
    {
      candidate = (RacerEntity *)find_nearest_longitudinal_track_object(
        (TrackInteractionObject *)state,
        RIDER_ATTACK_FALLBACK_TARGET_KIND,
        RIDER_ATTACK_TARGET_SEARCH_RANGE);
      state->interaction_data->attack_target = 0;
      return candidate;
    }

  state->interaction_data->attack_target = candidate->owner;
  return 0;
}


int
rider_update_mode_3(int rider_address)
{
  RacerEntity *rider;
  RacerEntity *state;
  RacerEntity *attack_state;
  RiderInteractionRuntime *interaction;
  RacerEntity *challenge;
  SignedDivisionResult random_roll;
  int use_alternate_variant;

  rider = (RacerEntity *)rider_address;
  state = rider->paired_state;
  interaction = state->interaction_data;

  rider_update_mode_4(rider_address);
  if(is_rider_attack_animation(rider) == false)
    {
      random_roll = signed_divide_with_remainder(
        (signed char)interaction->attack_kick_roll_divisor,
        (unsigned int)rand());
      if(random_roll.remainder != 0)
        {
          goto select_queued_attack;
        }
      challenge = (RacerEntity *)race_rider_state.challenge_object;
      if(challenge != 0 && challenge->paired_state == state)
        {
          goto select_queued_attack;
        }

      use_alternate_variant = false;
      attack_state = *(RacerEntity *volatile *)&rider->paired_state;
      attack_state->attack_style = RIDER_ATTACK_STYLE_KICK;
      goto select_attack;

select_queued_attack:
      attack_state = *(RacerEntity *volatile *)&rider->paired_state;
      attack_state->attack_style = attack_state->queued_attack_style;
      random_roll = signed_divide_with_remainder(
        (signed char)interaction->attack_alternate_roll_divisor,
        (unsigned int)rand());
      use_alternate_variant = 0;
      if(random_roll.remainder == 0)
        {
          use_alternate_variant = 1;
        }

select_attack:
      select_rider_attack_animation(
        rider, 0, (boolean)use_alternate_variant);
    }
  else
    {
      resolve_rider_attack_contact(rider);
    }

  return apply_ai_rider_navigation_control(rider);
}
