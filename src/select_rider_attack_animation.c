#include "rider_contact_runtime.h"
#include "rw_semantic_data.h"

enum RiderAttackAnimationValue
{
  RIDER_ATTACK_VARIANT_MASK = 0xFF,
  RIDER_ATTACK_RESPONSE_TARGET_BASE = 0,
  RIDER_ATTACK_RESPONSE_TARGET_MATCH = 1,
  RIDER_ATTACK_RESPONSE_RIDER = 3,
  RIDER_METRIC_COMPONENT_START = 0,
  RIDER_METRIC_COMPONENT_MIDDLE = 1,
  RIDER_METRIC_COMPONENT_END = 2
};

int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);

int
select_rider_attack_animation(RacerEntity *rider,
                              RacerEntity *target,
                              int          alternate_variant)
{
  RacerEntity *state;
  RacerEntity *attack_target;
  RiderInteractionRuntime *interaction;
  RiderInteractionRuntime *target_interaction;
  RiderAttackStyle attack_style;
  int attacks_right;
  int table_index;
  int animation_state;
  int target_extent;
  int current_extent;
  signed char metric_index;

  state = rider->paired_state;
  interaction = state->interaction_data;
  attack_target = interaction->attack_target;
  attack_style = (RiderAttackStyle)(signed char)state->attack_style;

  attacks_right = 1;
  if(target != 0)
    {
      if(target->position_x <= rider->position_x)
        {
          attacks_right = 0;
        }
    }
  else if(attack_target != 0)
    {
      if(attack_target->paired_state->position_x <= rider->position_x)
        {
          attacks_right = 0;
        }
    }
  else if(rider->orientation.steering_heading < 0)
    {
      attacks_right = 0;
    }

  table_index =
    ((alternate_variant & RIDER_ATTACK_VARIANT_MASK) *
     RIDER_ATTACK_STYLE_COUNT) + attack_style;
  if(attacks_right != 0)
    {
      animation_state = opponent_resource_group_b[table_index];
    }
  else
    {
      animation_state = opponent_resource_group_a[table_index];
    }

  if(attack_target != 0 &&
     (signed char)state->animation.primary_state != animation_state)
    {
      target_interaction = attack_target->paired_state->interaction_data;
      metric_index = interaction->metric_index;
      if(metric_index < RIDER_INTERACTION_METRIC_COUNT)
        {
          target_extent = target_interaction->target_metrics[metric_index]
                          .components[RIDER_METRIC_COMPONENT_END] +
                          target_interaction->target_metrics[metric_index]
                          .components[RIDER_METRIC_COMPONENT_MIDDLE] -
                          target_interaction->target_metrics[metric_index]
                          .components[RIDER_METRIC_COMPONENT_START];
          current_extent = target_interaction->current_metrics[metric_index]
                           .components[RIDER_METRIC_COMPONENT_END] +
                           target_interaction->current_metrics[metric_index]
                           .components[RIDER_METRIC_COMPONENT_MIDDLE] -
                           target_interaction->current_metrics[metric_index]
                           .components[RIDER_METRIC_COMPONENT_START];
          if(target_extent == current_extent)
            {
              accumulate_rider_contact_response(
                attack_target, rider,
                RIDER_ATTACK_RESPONSE_TARGET_MATCH);
            }
        }
      accumulate_rider_contact_response(
        attack_target, rider, RIDER_ATTACK_RESPONSE_TARGET_BASE);
      accumulate_rider_contact_response(
        rider, attack_target, RIDER_ATTACK_RESPONSE_RIDER);
    }

  return select_primary_rider_animation(
    &state->animation, animation_state);
}
