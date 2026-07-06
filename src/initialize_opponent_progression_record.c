#include "racer_creation_runtime.h"

enum OpponentProgressionSelectionValue
{
  STANDARD_OPPONENT_SLOT_COUNT = 9,
  CHALLENGE_RIDER_SLOT = 15,
  CHALLENGE_PROGRESSION_INDEX = 9,
  UNSIGNED_BYTE_MASK = 255
};

int
initialize_opponent_progression_record(OpponentProgressionRecord **destination,
                                       int                         selection_kind,
                                       int                         selected_reward,
                                       int                         rider_slot)
{
  int signed_slot;
  int profile_index;
  OpponentProgressionRecord *selected;

  signed_slot = (signed char)rider_slot;
  if(signed_slot < STANDARD_OPPONENT_SLOT_COUNT)
    {
      profile_index = race_mode_state.active_player_index;
      selected =
        &gRiderProgressionProfiles[profile_index][signed_slot];
    }
  else if(((unsigned int)signed_slot & UNSIGNED_BYTE_MASK) ==
          CHALLENGE_RIDER_SLOT)
    {
      profile_index = race_mode_state.active_player_index;
      selected = &gRiderProgressionProfiles
                 [profile_index][CHALLENGE_PROGRESSION_INDEX];
    }
  else
    {
      selected = &gChallengeProgressionProfiles
                 [signed_slot - STANDARD_OPPONENT_SLOT_COUNT];
      selected->fields.current_stage =
        (unsigned char)(signed_slot + 1);
    }

  *destination = selected;
  selected->fields.selection_kind = (unsigned char)selection_kind;
  selected->fields.selected_reward = selected_reward;
  return (int)&selected->fields.selected_reward;
}
