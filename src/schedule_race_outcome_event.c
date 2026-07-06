/* Reconstructed from the original function at 0x00009CD4. */



#include "division_helpers.h"
#include "rw_semantic_data.h"

enum RaceOutcomeConstant
{
  RACE_OUTCOME_EVENT_RESET = 1,
  RACE_OUTCOME_EVENT_BIKE_REPAIR = 4,
  RACE_OUTCOME_EVENT_FORFEIT = 5,
  RACE_OUTCOME_EVENT_FINISH = 0x7F,
  RACE_OUTCOME_MODE_RESET = 0,
  RACE_OUTCOME_MODE_PODIUM = 2,
  RACE_OUTCOME_MODE_NON_PODIUM = 3,
  RACE_OUTCOME_MODE_BIKE_REPAIR = 4,
  RACE_OUTCOME_MODE_FORFEIT = 5,
  RACE_OUTCOME_MODE_ADVANCE_LEVEL = 0x7E,
  RACE_SINGLE_COURSE_MODE = 1,
  RACE_BIKE_REPAIR_COST_PER_LEVEL = 400,
  RACE_CASH_AWARD_MULTIPLIER = 10,
  RACE_PODIUM_POSITION_COUNT = 3,
  RACE_COMPLETED_COURSE_MASK = 0x1F,
  RACE_COMPLETED_COURSE_BYTE_MASK = 0xFF,
  RACE_FINAL_VARIANT_INDEX = 4
};

void
schedule_race_outcome_event(int event,
                            int value)
{
  RiderProfile *profile;
  int top_byte;
  int selector;
  int mask;
  int index;
  SignedDivisionResult div_result;

  if(input_thread_state.deferred_event_ticks != 0)
    {
      return;
    }

  if(input_thread_state.deferred_event_pending != 0)
    {
      return;
    }

  if(event == RACE_OUTCOME_EVENT_RESET)
    {
      race_mode_state.outcome_mode = RACE_OUTCOME_MODE_RESET;
    }
  else
    {
      top_byte = race_mode_state.game_mode;
      selector = race_mode_state.active_player_index;
      profile = &player_profiles[selector];

      if(event == RACE_OUTCOME_EVENT_BIKE_REPAIR)
        {
          race_mode_state.outcome_mode = RACE_OUTCOME_MODE_BIKE_REPAIR;
          profile->balance -=
            (profile->level + 1) * RACE_BIKE_REPAIR_COST_PER_LEVEL;
          if(top_byte == RACE_SINGLE_COURSE_MODE)
            {
              profile->completed_courses = 0;
            }
          else
            {
              profile->completed_courses &=
                (unsigned char)~(1 << profile->current_variant);
            }
        }
      else if(event == RACE_OUTCOME_EVENT_FORFEIT)
        {
          race_mode_state.outcome_mode = RACE_OUTCOME_MODE_FORFEIT;
          index = profile->rider_style;
          div_result = signed_divide_by_ten_with_remainder(rider_style_purchase_prices[index]);
          profile->balance -= div_result.quotient;
          if(top_byte == RACE_SINGLE_COURSE_MODE)
            {
              profile->completed_courses = 0;
            }
          else
            {
              profile->completed_courses &=
                (unsigned char)~(1 << profile->current_variant);
            }
        }
      else if(event == RACE_OUTCOME_EVENT_FINISH)
        {
          index = race_mode_state.finish_position;
          profile->balance +=
            (profile->level + 1) * race_cash_awards[index] *
            RACE_CASH_AWARD_MULTIPLIER;
          if(index < RACE_PODIUM_POSITION_COUNT)
            {
              race_mode_state.outcome_mode = RACE_OUTCOME_MODE_PODIUM;
              mask = profile->completed_courses |
                     (1 << profile->current_variant);
              if((mask & RACE_COMPLETED_COURSE_BYTE_MASK) ==
                 RACE_COMPLETED_COURSE_MASK)
                {
                  race_mode_state.outcome_mode =
                    RACE_OUTCOME_MODE_ADVANCE_LEVEL;
                  profile->current_variant = RACE_FINAL_VARIANT_INDEX;
                  mask = 0;
                }
              profile->completed_courses = (unsigned char)mask;
            }
          else
            {
              race_mode_state.outcome_mode = RACE_OUTCOME_MODE_NON_PODIUM;
            }
        }
    }

  if(value <= 0)
    {
      input_thread_state.deferred_event_pending = 1;
    }
  else
    {
      input_thread_state.deferred_event_ticks = value;
    }
}
