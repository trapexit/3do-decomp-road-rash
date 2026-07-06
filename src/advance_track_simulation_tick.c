#include "task.h"

#include "bss_late_data.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"

#define TWO_TICK_EVENT_PERIOD       2
#define THREE_TICK_EVENT_PERIOD     3
#define TWO_TICK_EVENT_FLAG         1
#define THREE_TICK_EVENT_FLAG       2
#define TRACK_CELL_8_8              0x100
#define TRACK_FRACTION_BITS         8
#define TRACK_FRACTION_MASK         0xFF
#define RACER_STATUS_DISABLED       0x10

void
dispatch_pending_family_resource_request(void);
void
update_active_race_objects(void);
void
update_race_object_schedule(int use_track_windows);
void
update_road_view_motion(int current_tick);
void
advance_road_track_position(int distance);
void
advance_track_object_spawn_timers(int distance);

static
int
shift_track_fraction(int value)
{
  return (int)((unsigned int)value << TRACK_FRACTION_BITS);
}


static
int
multiply_wrapping_int(int left,
                      int right)
{
  return (int)((unsigned int)left * (unsigned int)right);
}


void
advance_track_simulation_tick(void)
{
  RacerEntity *player;
  RacerEntity *player_state;
  RoadSegmentLaneRuntime *previous_lane;
  RoadSegmentLaneRuntime *current_lane;
  int current_tick;
  int player_state_tick;
  int target_position;
  int applied_position;
  int track_delta;
  int metric_delta;
  int fraction;

  track_simulation_globals.motion_source.periodic_event_flags.value = 0;

  metric_delta = track_simulation_globals.two_tick_event_countdown - 1;
  track_simulation_globals.two_tick_event_countdown = metric_delta;
  if(metric_delta == 0)
    {
      track_simulation_globals.two_tick_event_countdown =
        TWO_TICK_EVENT_PERIOD;
      track_simulation_globals.motion_source.periodic_event_flags.value =
        TWO_TICK_EVENT_FLAG;
    }

  metric_delta = track_simulation_globals.three_tick_event_countdown - 1;
  track_simulation_globals.three_tick_event_countdown = metric_delta;
  if(metric_delta == 0)
    {
      track_simulation_globals.three_tick_event_countdown =
        THREE_TICK_EVENT_PERIOD;
      track_simulation_globals.motion_source.periodic_event_flags.value |=
        THREE_TICK_EVENT_FLAG;
    }

  dispatch_pending_family_resource_request();
  if(track_simulation_globals.simulation_active.value != 0)
    {
      update_active_race_objects();
    }
  else
    {
      update_race_object_schedule(0);
    }

  player = (RacerEntity *)race_rider_state.player_object;
  player_state = player->paired_state;
  current_tick = (int)frame_tick;
  player_state_tick = player_state->last_update_tick;
  if(player_state_tick == current_tick)
    {
      update_road_view_motion(current_tick);
    }

  SendSignal(input_thread_state.task_item, input_thread_state.wake_signal);

  previous_lane = player_state->secondary_track;
  current_lane = player_state->primary_track;
  target_position =
    track_simulation_globals.motion_source.packed_track_position;
  applied_position = track_simulation_globals.applied_track_position;
  track_delta = target_position - applied_position;

  while(track_delta > TRACK_CELL_8_8)
    {
      advance_road_track_position(TRACK_CELL_8_8);
      track_delta -= TRACK_CELL_8_8;
    }
  while(track_delta < -TRACK_CELL_8_8)
    {
      advance_road_track_position(-TRACK_CELL_8_8);
      track_delta += TRACK_CELL_8_8;
    }

  advance_road_track_position(track_delta);

  applied_position = track_simulation_globals.applied_track_position;
  track_delta = target_position - applied_position;
  advance_track_object_spawn_timers(track_delta);

  if(track_delta != 0 && track_delta <= TRACK_CELL_8_8 &&
     track_delta >= -TRACK_CELL_8_8 &&
     (player->status_flags & RACER_STATUS_DISABLED) == 0)
    {
      if((target_position >> TRACK_FRACTION_BITS) ==
         (applied_position >> TRACK_FRACTION_BITS))
        {
          metric_delta = multiply_wrapping_int(
            current_lane->path_curvature_step,
            shift_track_fraction(track_delta));
          track_simulation_globals.motion_source.road_scroll_phase +=
            metric_delta;
        }
      else if(track_delta > 0)
        {
          fraction = applied_position & TRACK_FRACTION_MASK;
          metric_delta = multiply_wrapping_int(
            previous_lane->path_curvature_step,
            shift_track_fraction(TRACK_CELL_8_8 - fraction));
          metric_delta +=
            track_simulation_globals.motion_source.road_scroll_phase;
          track_simulation_globals.motion_source.road_scroll_phase =
            metric_delta;

          metric_delta += multiply_wrapping_int(
            current_lane->path_curvature_step,
            shift_track_fraction(target_position & TRACK_FRACTION_MASK));
          track_simulation_globals.motion_source.road_scroll_phase =
            metric_delta;
        }
      else
        {
          fraction = applied_position & TRACK_FRACTION_MASK;
          metric_delta =
            track_simulation_globals.motion_source.road_scroll_phase -
            multiply_wrapping_int(previous_lane->path_curvature_step,
                                  shift_track_fraction(fraction));
          track_simulation_globals.motion_source.road_scroll_phase =
            metric_delta;

          metric_delta -= multiply_wrapping_int(
            current_lane->path_curvature_step,
            shift_track_fraction(
              TRACK_CELL_8_8 -
              (target_position & TRACK_FRACTION_MASK)));
          track_simulation_globals.motion_source.road_scroll_phase =
            metric_delta;
        }
    }

  track_simulation_globals.applied_track_position = target_position;
}


#undef TWO_TICK_EVENT_PERIOD
#undef THREE_TICK_EVENT_PERIOD
#undef TWO_TICK_EVENT_FLAG
#undef THREE_TICK_EVENT_FLAG
#undef TRACK_CELL_8_8
#undef TRACK_FRACTION_BITS
#undef TRACK_FRACTION_MASK
#undef RACER_STATUS_DISABLED
