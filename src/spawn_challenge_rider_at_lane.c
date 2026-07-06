#include "intrusive_list.h"
#include "racer_creation_runtime.h"
#include "rw_semantic_data.h"

enum ChallengeRiderSpawnValue
{
  CHALLENGE_RESOURCE_ITEM_COUNT = 8,
  CHALLENGE_RANK_RAMP_COUNT = 4,
  CHALLENGE_PACING_BASE = 0x7D00,
  CHALLENGE_PROFILE_PACING_STEP = 3000,
  CHALLENGE_RANK_PACING_STEP = 2000,
  CHALLENGE_PACING_SCALE_SHIFT = 1,
  CHALLENGE_HALF_SIGN_SHIFT = 31,
  CHALLENGE_HALF_SHIFT = 1,
  CHALLENGE_METRIC_COPY_COMPONENT = 1,
  CHALLENGE_METRIC_PACING_COMPONENT = 2
};

int *
spawn_challenge_rider_at_lane(RoadSegmentLaneRuntime *track_lane)
{
  AiRacerObject *challenge;
  RacerEntity *challenge_rider;
  RacerEntity *challenge_state;
  RacerEntity *player;
  RacerEntity *player_state;
  RiderInteractionRuntime *challenge_interaction;
  RiderInteractionRuntime *player_interaction;
  RiderInteractionMetric *metric;
  int resource_index;
  int metric_index;
  int profile_index;
  int profile_level;
  int player_rank;
  int rank_slot;
  int player_metric_index;
  int challenge_metric_index;
  int pacing_value;
  int previous_challenge_index;
  void *previous_resource_item;

  if(track_lane == 0 || race_rider_state.player_object == 0 ||
     race_mode_state.active_player_index < 0 ||
     race_mode_state.active_player_index >= PLAYER_PROFILE_COUNT)
    {
      return 0;
    }
  if(race_rider_state.challenge_object != 0)
    {
      return (int *)(void *)track_lane;
    }

  profile_index = race_mode_state.active_player_index;
  profile_level = player_profiles[profile_index].level;
  if(profile_level < 0 ||
     profile_level >= RIDER_PROGRESSION_LEVEL_COUNT)
    {
      return 0;
    }
  player = (RacerEntity *)race_rider_state.player_object;
  player_state = player->paired_state;
  if(player_state == 0 || player_state->interaction_data == 0)
    {
      return 0;
    }
  player_interaction = player_state->interaction_data;
  player_metric_index = player_interaction->metric_index;
  if(player_metric_index < 0 ||
     player_metric_index >= RIDER_INTERACTION_METRIC_COUNT)
    {
      return 0;
    }

  resource_index =
    (signed char)race_rider_state.active_challenge_index;
  if(resource_index < 0 || resource_index >= CHALLENGE_RESOURCE_ITEM_COUNT)
    {
      return 0;
    }
  previous_challenge_index = race_rider_state.active_challenge_index;
  previous_resource_item = rider_resource_items[RACE_RIDER_COUNT - 1];
  rider_resource_items[RACE_RIDER_COUNT - 1] =
    challenge_resource_items[resource_index];
  resource_index = (signed char)(resource_index + 1);
  race_rider_state.active_challenge_index =
    (unsigned char)resource_index;
  if(resource_index >= CHALLENGE_RESOURCE_ITEM_COUNT)
    {
      race_rider_state.active_challenge_index = 0;
    }

  challenge = create_rider_pair(
    &challenge_object_template, &challenge_state_template,
    track_lane, 0,
    RACER_ROLE_CHALLENGE, RACE_RIDER_COUNT - 1);
  if(challenge == 0)
    {
      race_rider_state.active_challenge_index =
        (unsigned char)previous_challenge_index;
      rider_resource_items[RACE_RIDER_COUNT - 1] = previous_resource_item;
      return 0;
    }

  challenge_rider = (RacerEntity *)challenge;
  challenge_rider->velocity.forward = 0;
  challenge_state = challenge_rider->paired_state;
  if(challenge_state == 0 || challenge_state->interaction_data == 0)
    {
      return_pooled_node_to_owner(&challenge_rider->list_node);
      if(challenge_state != 0)
        {
          return_pooled_node_to_owner(&challenge_state->list_node);
        }
      race_rider_state.active_challenge_index =
        (unsigned char)previous_challenge_index;
      rider_resource_items[RACE_RIDER_COUNT - 1] = previous_resource_item;
      return 0;
    }
  challenge_interaction = challenge_state->interaction_data;
  challenge_metric_index = challenge_interaction->metric_index;
  if(challenge_metric_index < 0 ||
     challenge_metric_index >= RIDER_INTERACTION_METRIC_COUNT)
    {
      return_pooled_node_to_owner(&challenge_rider->list_node);
      return_pooled_node_to_owner(&challenge_state->list_node);
      race_rider_state.active_challenge_index =
        (unsigned char)previous_challenge_index;
      rider_resource_items[RACE_RIDER_COUNT - 1] = previous_resource_item;
      return 0;
    }
  race_rider_state.challenge_object = challenge;

  metric_index = 0;
  do
    {
      challenge_interaction->current_metrics[metric_index]
      .components[CHALLENGE_METRIC_COPY_COMPONENT] =
        challenge_interaction->target_metrics[metric_index]
        .components[CHALLENGE_METRIC_COPY_COMPONENT];
      metric_index = (signed char)(metric_index + 1);
    } while(metric_index < RIDER_INTERACTION_METRIC_COUNT);

  challenge_interaction->target_speed_limit =
    challenge_interaction->pacing_values[profile_level];

  player_rank = player->paired_state->race_rank;
  rank_slot = 0;
  if(player_rank < CHALLENGE_RANK_RAMP_COUNT)
    {
      rank_slot = CHALLENGE_RANK_RAMP_COUNT - player_rank;
    }
  pacing_value =
    (CHALLENGE_PACING_BASE -
     (profile_level * CHALLENGE_PROFILE_PACING_STEP +
      rank_slot * CHALLENGE_RANK_PACING_STEP)) <<
      CHALLENGE_PACING_SCALE_SHIFT;
  metric = &challenge_interaction->target_metrics[player_metric_index];
  metric->components[CHALLENGE_METRIC_PACING_COMPONENT] = pacing_value;
  metric = &challenge_interaction->current_metrics[player_metric_index];
  metric->components[CHALLENGE_METRIC_PACING_COMPONENT] = pacing_value;

  player_rank = player->paired_state->race_rank;
  rank_slot = 0;
  if(player_rank < CHALLENGE_RANK_RAMP_COUNT)
    {
      rank_slot = CHALLENGE_RANK_RAMP_COUNT - player_rank;
    }
  pacing_value = CHALLENGE_PACING_BASE -
                 (profile_level * CHALLENGE_PROFILE_PACING_STEP +
                  rank_slot * CHALLENGE_RANK_PACING_STEP);
  pacing_value +=
    (pacing_value +
     (int)((unsigned int)pacing_value >> CHALLENGE_HALF_SIGN_SHIFT)) >>
    CHALLENGE_HALF_SHIFT;

  metric = &player_interaction->target_metrics[challenge_metric_index];
  metric->components[CHALLENGE_METRIC_PACING_COMPONENT] = pacing_value;
  metric = &player_interaction->current_metrics[challenge_metric_index];
  metric->components[CHALLENGE_METRIC_PACING_COMPONENT] = pacing_value;

  return &metric->components[CHALLENGE_METRIC_PACING_COMPONENT];
}
