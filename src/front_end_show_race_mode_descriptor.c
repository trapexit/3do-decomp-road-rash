#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "front_end_transition_runtime.h"
#include "rw_semantic_data.h"

enum
{
  FRONT_END_OUTCOME_PROFILE = 2,
  FRONT_END_OUTCOME_SHARED = 3,
  FRONT_END_OUTCOME_MODE_FOUR = 4,
  FRONT_END_OUTCOME_MODE_FIVE = 5,
  FRONT_END_OUTCOME_SPECIAL_PROFILE = 0x7E,
  FRONT_END_SPECIAL_PROFILE_LEVEL = 4
};

void
front_end_show_race_mode_descriptor(void)
{
  RiderProfile *active_profile;
  FrontEndCelDescriptor *selected;
  int shared_resource_id;
  int outcome_mode;
  int active_player_index;

  outcome_mode = race_mode_state.outcome_mode;
  shared_resource_id = front_end_preview_state.selected_resource_id;
  selected = 0;

  if(outcome_mode == FRONT_END_OUTCOME_SHARED)
    {
      selected = front_end_cel_group_288;
      selected->resource_id = shared_resource_id;
    }
  else if(outcome_mode == FRONT_END_OUTCOME_PROFILE ||
          outcome_mode == FRONT_END_OUTCOME_SPECIAL_PROFILE)
    {
      active_player_index = race_mode_state.active_player_index;
      active_profile = &player_profiles[active_player_index];
      if(active_profile->level == FRONT_END_SPECIAL_PROFILE_LEVEL &&
         outcome_mode == FRONT_END_OUTCOME_SPECIAL_PROFILE)
        {
          selected = front_end_cel_group_287;
        }
      else
        {
          selected = front_end_cel_group_286;
          selected->resource_id = shared_resource_id;
        }
    }
  else if(outcome_mode == FRONT_END_OUTCOME_MODE_FOUR)
    {
      if(race_mode_state.game_mode != FRONT_END_GAME_MODE_STANDARD)
        {
          selected = front_end_cel_group_289;
        }
      else
        {
          active_player_index = race_mode_state.active_player_index;
          active_profile = &player_profiles[active_player_index];
          if(active_profile->balance < 0)
            {
              selected = front_end_cel_group_291;
            }
          else
            {
              selected = front_end_cel_group_290;
              if(select_front_end_rider_resource(
                   selected,
                   FRONT_END_RIDER_MODE_FOUR_BASE_RESOURCE) == 0)
                {
                  return;
                }
            }
        }
      selected->resource_id = shared_resource_id;
    }
  else if(outcome_mode == FRONT_END_OUTCOME_MODE_FIVE)
    {
      if(race_mode_state.game_mode != FRONT_END_GAME_MODE_STANDARD)
        {
          selected = front_end_cel_group_292;
        }
      else
        {
          active_player_index = race_mode_state.active_player_index;
          active_profile = &player_profiles[active_player_index];
          if(active_profile->balance < 0)
            {
              selected = front_end_cel_group_294;
            }
          else
            {
              selected = front_end_cel_group_293;
              select_front_end_rider_resource(
                selected, FRONT_END_RIDER_MODE_FIVE_BASE_RESOURCE);
            }
        }
      selected->resource_id = shared_resource_id;
    }

  if(selected != 0)
    {
      apply_front_end_cel_descriptors(selected);
    }
}
