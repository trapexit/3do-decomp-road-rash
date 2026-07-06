#include "race_audio_runtime.h"
#include "race_object_lifecycle.h"
#include "racer_runtime.h"
#include "roadside_collision_runtime.h"
#include "rw_semantic_data.h"

int
racer_object_update(RacerEntity *rider);
int
ai_racer_control(int object_address);
void
player_rider_control(int object_address);
void
ai_racer_state_control(int object_address);
void
sort_race_object_schedule_by_track_position(IntrusiveListHeader *list);

void
update_active_race_objects(void)
{
  IntrusiveListHeader *list;
  RacerEntity *node;
  RacerEntity *end;
  RacerEntity *player;
  RacerEntity *paired_state;
  RacerEntity *nearest_engine_object;
  RacerEntity *second_nearest_engine_object;
  int tick;

  tick = frame_tick;

  if(track_simulation_globals.enabled != 0)
    {
      node = (RacerEntity *)race_object_scheduler.first_visible_object;
      end = (RacerEntity *)race_object_scheduler.last_visible_object;
      while(node != end)
        {
          if(node->next_update_tick <= tick)
            {
              update_scheduled_race_object(node, tick, 0);
            }
          node = (RacerEntity *)node->list_node.links.previous;
        }
    }
  else
    {
      player = (RacerEntity *)race_rider_state.player_object;
      player->time_step = tick - player->last_update_tick;
      player->last_update_tick = tick;
      racer_object_update(player);
      ai_racer_control((int)player);
      player->next_update_tick = tick + 1;

      paired_state = player->paired_state;
      paired_state->time_step = tick - paired_state->last_update_tick;
      paired_state->last_update_tick = tick;
      player_rider_control((int)paired_state);
      ai_racer_state_control((int)paired_state);
      paired_state->next_update_tick = tick + 1;

      if((track_simulation_globals.motion_source.periodic_event_flags.value &
          1) != 0)
        {
          find_nearest_engine_audio_objects(
            &nearest_engine_object, &second_nearest_engine_object);
          if(nearest_engine_object != 0)
            {
              update_scheduled_race_object(
                nearest_engine_object, tick, 0);
            }
          if(second_nearest_engine_object != 0)
            {
              update_scheduled_race_object(
                second_nearest_engine_object, tick, 0);
            }
        }
    }

  list = race_object_scheduler.object_list;
  sort_race_object_schedule_by_track_position(list);
  if(resolve_active_racer_collisions(list) != 0)
    {
      sort_race_object_schedule_by_track_position(list);
    }
}
