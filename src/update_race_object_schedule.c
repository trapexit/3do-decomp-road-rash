#include "ai_racer_runtime.h"
#include "race_object_lifecycle.h"
#include "racer_runtime.h"
#include "roadside_collision_runtime.h"
#include "rw_semantic_data.h"

enum RaceObjectScheduleConstants
{
  RACE_OBJECT_SCHEDULE_FAR_FORWARD_OFFSET = 0x1F00,
  RACE_OBJECT_SCHEDULE_NEAR_FORWARD_OFFSET = 0x0F00,
  RACE_OBJECT_SCHEDULE_REAR_OFFSET = 0x400,
  RACE_OBJECT_SCHEDULE_RECENT_TICK_DELTA = 2,
  RACE_OBJECT_SCHEDULE_CROSSING_FLAGS = 0x0C,
  RACE_OBJECT_SCHEDULE_ACTIVE_FLAGS = 0x3C,
  RACE_OBJECT_SCHEDULE_LONG_INTERVAL = 6
};

void
sort_race_object_schedule_by_track_position(IntrusiveListHeader *list);

__inline
static
int
race_object_has_track_segment(RacerEntity *object)
{
  return object->track_segment != 0;
}


__inline
static
RacerEntity *
previous_scheduled_race_object(RacerEntity *object)
{
  return (RacerEntity *)object->list_node.links.previous;
}


__inline
static
void
update_scheduled_object_if_needed(RacerEntity *object,
                                  int          tick,
                                  int          require_track_segment)
{
  update_scheduled_race_object(
    object, tick,
    require_track_segment && race_object_has_track_segment(object));
}


void
update_race_object_schedule(int use_visibility_bands)
{
  IntrusiveListHeader *list;
  RacerEntity *node;
  RacerEntity *end;
  RacerEntity *player;
  int tick;
  int track_limit;
  int recent_tick;
  int update_flags;

  if(race_object_scheduler.removal_pending != 0)
    {
      dispose_pending_race_objects();
    }

  tick = frame_tick;
  list = race_object_scheduler.object_list;
  node = (RacerEntity *)list->tail_sentinel->previous;

  if(use_visibility_bands == 0)
    {
      end = (RacerEntity *)list->head_sentinel;
      while(node != end)
        {
          if(node->next_update_tick <= tick)
            {
              update_scheduled_object_if_needed(node, tick, 1);
            }
          node = previous_scheduled_race_object(node);
        }
    }
  else
    {
      race_object_scheduler.object_update_tick = tick;

      track_limit = track_runtime_globals.current_track_position +
                    RACE_OBJECT_SCHEDULE_FAR_FORWARD_OFFSET;
      while(previous_scheduled_race_object(node) != 0 &&
            node->position_z >= track_limit)
        {
          update_flags = node->collision_class;
          if(node->next_update_tick <= tick ||
             (node->last_update_tick < tick &&
              (update_flags & RACE_OBJECT_SCHEDULE_CROSSING_FLAGS) != 0 &&
              race_object_has_track_segment(node)))
            {
              update_scheduled_object_if_needed(node, tick, 1);
            }
          node = previous_scheduled_race_object(node);
        }
      race_object_scheduler.first_visible_object =
        (IntrusiveListNode *)node;

      track_limit = track_runtime_globals.current_track_position +
                    RACE_OBJECT_SCHEDULE_NEAR_FORWARD_OFFSET;
      recent_tick = tick - RACE_OBJECT_SCHEDULE_RECENT_TICK_DELTA;
      while(previous_scheduled_race_object(node) != 0 &&
            node->position_z >= track_limit)
        {
          update_flags = node->collision_class;
          if(node->next_update_tick <= tick ||
             (node->last_update_tick < tick &&
              (update_flags & RACE_OBJECT_SCHEDULE_CROSSING_FLAGS) != 0) ||
             (node->last_update_tick < recent_tick &&
              (update_flags & RACE_OBJECT_SCHEDULE_ACTIVE_FLAGS) != 0))
            {
              update_scheduled_object_if_needed(node, tick, 1);
            }
          node = previous_scheduled_race_object(node);
        }

      track_limit = track_runtime_globals.current_track_position -
                    RACE_OBJECT_SCHEDULE_REAR_OFFSET;
      while(previous_scheduled_race_object(node) != 0 &&
            node->position_z >= track_limit)
        {
          update_flags = node->collision_class;
          if(node->next_update_tick <= tick ||
             (node->last_update_tick < tick &&
              (update_flags & RACE_OBJECT_SCHEDULE_ACTIVE_FLAGS) != 0))
            {
              update_scheduled_race_object(node, tick, 0);
            }
          node = previous_scheduled_race_object(node);
        }
      race_object_scheduler.last_visible_object =
        (IntrusiveListNode *)node;

      if(previous_scheduled_race_object(node) != 0)
        {
          do
            {
              update_flags = node->collision_class;
              if(node->next_update_tick <= tick ||
                 (node->last_update_tick < tick &&
                  (update_flags &
                   RACE_OBJECT_SCHEDULE_CROSSING_FLAGS) != 0 &&
                  race_object_has_track_segment(node)))
                {
                  update_scheduled_object_if_needed(node, tick, 1);
                }
              node = previous_scheduled_race_object(node);
            } while(previous_scheduled_race_object(node) != 0);
        }

      while(race_object_scheduler.first_visible_object !=
            race_object_scheduler.last_visible_object &&
            node->update_interval_ticks >
            RACE_OBJECT_SCHEDULE_LONG_INTERVAL &&
            (node->status_flags & RACE_OBJECT_SCHEDULE_ACTIVE_FLAGS) == 0)
        {
          node = previous_scheduled_race_object(node);
        }
    }

  race_object_scheduler.previous_frame_counter =
    race_object_scheduler.current_frame_counter;
  race_object_scheduler.current_frame_counter = 0;

  if(race_input_latch_state.event_latched == 0)
    {
      player = (RacerEntity *)race_rider_state.player_object;
      player->paired_state->next_update_tick = tick + 1;
      player->next_update_tick = tick + 1;
    }

  sort_race_object_schedule_by_track_position(list);
  if(resolve_active_racer_collisions(list) != 0)
    {
      sort_race_object_schedule_by_track_position(list);
    }

  adjust_ai_racer_pacing_by_rank();
}
