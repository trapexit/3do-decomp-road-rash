#include "intrusive_list.h"
#include "rw_semantic_data.h"

void
clear_rider_animation_resource_pairs(void);
void
unload_bike_specs_resource(void);

void
destroy_race_rider_runtime(void)
{
  int rider_index;

  clear_rider_animation_resource_pairs();

  if(race_rider_state.object_pool != 0)
    {
      destroy_intrusive_object_pool(
        (IntrusiveListStorage *)race_rider_state.object_pool);
    }
  if(race_rider_state.state_pool != 0)
    {
      destroy_intrusive_object_pool(
        (IntrusiveListStorage *)race_rider_state.state_pool);
    }

  race_rider_state.player_object = 0;
  race_rider_state.challenge_object = 0;
  race_rider_state.object_pool = 0;
  race_rider_state.state_pool = 0;
  for(rider_index = 0; rider_index < RACE_RIDER_COUNT; rider_index++)
    {
      race_rider_state.rider_objects[rider_index] = 0;
      race_rider_state.rider_states[rider_index] = 0;
    }

  unload_bike_specs_resource();
}
