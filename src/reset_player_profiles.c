
#include "rw_semantic_data.h"

#define RIDER_OPPONENT_SLOT_COUNT 4
#define RIDER_VALUE_MASK 0xFF
#define SECOND_PLAYER_DEFAULT_RIDER_TYPE 9

static
void
reset_profile(RiderProfile *profile,
              int           reset_rider_type,
              int           default_rider_type)
{
  int index;
  int slot;

  if(reset_rider_type != 0)
    {
      profile->rider_type = (signed char)default_rider_type;
    }

  index = profile->rider_type;
  profile->rider_style = (signed char)rider_style_defaults[index][0];
  profile->current_variant = 0;
  profile->level = 0;
  profile->completed_courses = 0;
  profile->balance = rider_starting_balances[index];

  for(slot = 0; slot < RIDER_OPPONENT_SLOT_COUNT; slot++)
    {
      profile->opponent_rider_types[slot] = 0;
      profile->opponent_resource_ids[slot] = 0;
      profile->opponent_styles[slot] = 0;
    }
}


void
reset_player_profiles(int reset_player_one,
                      int reset_player_two,
                      int reset_rider_type)
{
  if((reset_player_one & RIDER_VALUE_MASK) != 0)
    {
      reset_profile(&player_profiles[0],
                    reset_rider_type & RIDER_VALUE_MASK, 0);
    }

  if((reset_player_two & RIDER_VALUE_MASK) == 0)
    {
      return;
    }

  reset_profile(&player_profiles[1],
                reset_rider_type & RIDER_VALUE_MASK,
                SECOND_PLAYER_DEFAULT_RIDER_TYPE);
}


#undef RIDER_OPPONENT_SLOT_COUNT
#undef RIDER_VALUE_MASK
#undef SECOND_PLAYER_DEFAULT_RIDER_TYPE
