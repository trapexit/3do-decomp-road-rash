#include "race_object_lifecycle.h"
#include "rw_semantic_data.h"
#include "track_object_runtime.h"

enum RaceObjectUpdateConstants
{
  RACE_OBJECT_DIVIDE_BY_TWO_SHIFT = 1,
  RACE_OBJECT_DIVIDE_BY_FOUR_SHIFT = 2,
  RACE_OBJECT_TRACK_FRACTION_SHIFT = 8,
  RACE_OBJECT_TRACK_FRACTION_BIAS = 0xFF,
  RACE_OBJECT_BASE_UPDATE_INTERVAL = 2,
  RACE_OBJECT_SEGMENT_SELF_LINK = 2,
  RACE_OBJECT_CLOSE_DELTA_MAX = 3,
  RACE_OBJECT_REVERSE_DELTA_MIN = -2,
  RACE_OBJECT_SPECIAL_REVERSE_DELTA_MIN = -1,
  RACE_OBJECT_WIDE_FORWARD_DELTA = 8,
  RACE_OBJECT_WIDE_REVERSE_DELTA = -4,
  RACE_OBJECT_NEARBY_DISTANCE = 0x7D0,
  RACE_OBJECT_NEAR_DELTA_MAX = 0x26,
  RACE_OBJECT_NEAR_DELTA_MIN = -3,
  RACE_OBJECT_NEAR_INTERVAL_MAX = 6,
  RACE_OBJECT_FAR_INTERVAL_SCALE = 4,
  RACE_OBJECT_FAR_INTERVAL_MAX = 0x3C,
  RACE_OBJECT_PAYLOAD_RECENT_TICK = 0x12C,
  RACE_OBJECT_PAYLOAD_NEAR_DISTANCE = 0x1800,
  RACE_OBJECT_PAYLOAD_MID_DISTANCE = 0x1000,
  RACE_OBJECT_PAYLOAD_CLOSE_DISTANCE = 0x800,
  RACE_OBJECT_PAYLOAD_INTERVAL_MAX = 8,
  RACE_OBJECT_UPDATE_FLAG_CROSSING = 0x0C,
  RACE_OBJECT_UPDATE_FLAG_SPECIAL = 0x10,
  RACE_OBJECT_UPDATE_FLAG_ANY = 0x3C,
  RACE_OBJECT_MISSING_PAYLOAD_INTERVAL = 0x1E,
  RACE_OBJECT_WINDOW_LEFT_MARGIN = 0x1E00,
  RACE_OBJECT_WINDOW_RIGHT_MARGIN = 0x800,
  RACE_OBJECT_LATERAL_POSITION_LIMIT = 0xC8000,
  RACE_OBJECT_BOOLEAN_MASK = 0xFF,
  RACE_OBJECT_RESET_TRACK_MISMATCH = 0,
  RACE_OBJECT_RESET_LATERAL_BOUNDS = 1
};

typedef struct RoadSegmentResourcePosition
{
  unsigned char reserved_00[0x08];
  int track_position;
} RoadSegmentResourcePosition;

__inline
static
int
divide_by_two_towards_zero(int value)
{
  if(value < 0)
    {
      value += 1;
    }
  return value >> RACE_OBJECT_DIVIDE_BY_TWO_SHIFT;
}


__inline
static
int
divide_by_four_towards_zero(int value)
{
  if(value < 0)
    {
      value += RACE_OBJECT_CLOSE_DELTA_MAX;
    }
  return value >> RACE_OBJECT_DIVIDE_BY_FOUR_SHIFT;
}


__inline
static
int
track_units_towards_zero(int value)
{
  if(value < 0)
    {
      value += RACE_OBJECT_TRACK_FRACTION_BIAS;
    }
  return value >> RACE_OBJECT_TRACK_FRACTION_SHIFT;
}


__inline
static
void
reset_race_object(RacerEntity *object,
                  int          reason)
{
  if(object->reset_object != 0)
    {
      object->reset_object(object, reason);
    }
  else
    {
      schedule_race_object_removal(object);
    }
}


static
int
calculate_special_update_interval(RacerEntity *object,
                                  int          track_delta)
{
  RacerEntity *player;
  TrackObject *track_object;
  int interval;
  int mode;
  int distance;

  interval = RACE_OBJECT_BASE_UPDATE_INTERVAL;
  track_object = (TrackObject *)object;
  mode = track_object->mode;
  if((mode & 1) != 0)
    {
      if(track_delta > RACE_OBJECT_CLOSE_DELTA_MAX)
        {
          interval += divide_by_four_towards_zero(track_delta + 1);
        }
      else if(track_delta < RACE_OBJECT_SPECIAL_REVERSE_DELTA_MIN)
        {
          interval = divide_by_two_towards_zero(-track_delta - 1) +
                     RACE_OBJECT_BASE_UPDATE_INTERVAL;
        }
    }
  else if(mode == 0)
    {
      if(track_delta > RACE_OBJECT_CLOSE_DELTA_MAX)
        {
          interval += divide_by_four_towards_zero(
            track_delta - RACE_OBJECT_CLOSE_DELTA_MAX);
        }
      else if(track_delta < RACE_OBJECT_REVERSE_DELTA_MIN)
        {
          interval = divide_by_two_towards_zero(
            -track_delta + RACE_OBJECT_REVERSE_DELTA_MIN) +
                     RACE_OBJECT_BASE_UPDATE_INTERVAL;
        }
      else
        {
          player = (RacerEntity *)race_rider_state.player_object;
          distance = player->world_velocity.forward -
                     object->world_velocity.forward;
          if(distance < 0)
            {
              distance = -distance;
            }
          if(distance < RACE_OBJECT_NEARBY_DISTANCE)
            {
              interval = 1;
            }
        }
    }
  else
    {
      if(track_delta > RACE_OBJECT_WIDE_FORWARD_DELTA)
        {
          interval += divide_by_four_towards_zero(track_delta + 1);
        }
      else if(track_delta < RACE_OBJECT_WIDE_REVERSE_DELTA)
        {
          interval = divide_by_two_towards_zero(
            -track_delta + RACE_OBJECT_WIDE_REVERSE_DELTA) +
                     RACE_OBJECT_BASE_UPDATE_INTERVAL;
        }
    }

  if(object->velocity.forward < RACE_OBJECT_NEARBY_DISTANCE)
    {
      interval++;
    }

  if(track_delta < RACE_OBJECT_NEAR_DELTA_MAX &&
     track_delta > RACE_OBJECT_NEAR_DELTA_MIN)
    {
      if(interval >= RACE_OBJECT_NEAR_INTERVAL_MAX)
        {
          interval = RACE_OBJECT_NEAR_INTERVAL_MAX;
        }
    }
  else if(interval * RACE_OBJECT_FAR_INTERVAL_SCALE <
          RACE_OBJECT_FAR_INTERVAL_MAX)
    {
      interval *= RACE_OBJECT_FAR_INTERVAL_SCALE;
    }
  else
    {
      interval = RACE_OBJECT_FAR_INTERVAL_MAX;
    }

  return interval;
}


static
int
calculate_payload_update_interval(RacerEntity *object,
                                  int          tick,
                                  int          track_delta)
{
  RacerEntity *player;
  int interval;
  int distance;

  interval = 1;
  if(track_delta > RACE_OBJECT_CLOSE_DELTA_MAX)
    {
      interval += divide_by_four_towards_zero(track_delta + 1);
    }
  else if(track_delta < RACE_OBJECT_REVERSE_DELTA_MIN)
    {
      interval += divide_by_four_towards_zero(
        RACE_OBJECT_REVERSE_DELTA_MIN - track_delta);
    }

  distance = tick - RACE_OBJECT_PAYLOAD_RECENT_TICK;
  if(distance < 0)
    {
      distance = object->velocity.forward -
                 RACE_OBJECT_PAYLOAD_NEAR_DISTANCE;
    }
  if(distance < 0)
    {
      player = (RacerEntity *)race_rider_state.player_object;
      if(object != player && object != player->paired_state)
        {
          interval++;
          distance = object->velocity.forward;
          if(distance < RACE_OBJECT_PAYLOAD_MID_DISTANCE)
            {
              interval++;
            }
          if(distance < RACE_OBJECT_PAYLOAD_CLOSE_DISTANCE)
            {
              interval += RACE_OBJECT_BASE_UPDATE_INTERVAL;
            }
        }
    }

  if(interval > RACE_OBJECT_PAYLOAD_INTERVAL_MAX)
    {
      interval = RACE_OBJECT_PAYLOAD_INTERVAL_MAX;
    }
  return interval;
}


__inline
static
int
calculate_general_update_interval(int track_delta)
{
  if(track_delta > RACE_OBJECT_CLOSE_DELTA_MAX)
    {
      return divide_by_two_towards_zero(track_delta + 1);
    }
  if(track_delta < RACE_OBJECT_REVERSE_DELTA_MIN)
    {
      return 1 - track_delta;
    }
  return 1;
}


__inline
static
void
refresh_race_object_update_interval(RacerEntity *object,
                                    int          tick)
{
  int update_flags;
  int track_delta;

  update_flags = object->collision_class;
  if((update_flags & RACE_OBJECT_UPDATE_FLAG_ANY) == 0)
    {
      return;
    }

  track_delta = track_units_towards_zero(
    object->position_z - track_runtime_globals.current_track_position);

  if(update_flags == RACE_OBJECT_UPDATE_FLAG_SPECIAL)
    {
      object->update_interval_ticks =
        calculate_special_update_interval(object, track_delta);
    }
  else if((update_flags & RACE_OBJECT_UPDATE_FLAG_CROSSING) == 0)
    {
      object->update_interval_ticks =
        calculate_general_update_interval(track_delta);
    }
  else if(object->track_segment == 0)
    {
      object->update_interval_ticks = RACE_OBJECT_MISSING_PAYLOAD_INTERVAL;
    }
  else
    {
      object->update_interval_ticks =
        calculate_payload_update_interval(object, tick, track_delta);
    }
}


__inline
static
int
race_object_inside_active_window(RacerEntity *object)
{
  int left_margin;
  int right_margin;
  int position;

  if((object->collision_class & RACE_OBJECT_UPDATE_FLAG_CROSSING) == 0)
    {
      left_margin = 0;
      right_margin = 0;
    }
  else
    {
      left_margin = RACE_OBJECT_WINDOW_LEFT_MARGIN;
      right_margin = RACE_OBJECT_WINDOW_RIGHT_MARGIN;
    }

  position = object->position_z;
  return (track_runtime_globals.right_bound - position >= left_margin) &&
         (position - track_runtime_globals.left_bound >= right_margin);
}


void
update_scheduled_race_object(RacerEntity *object,
                             int          tick,
                             int          enforce_visibility_window)
{
  RoadSegmentLaneRuntime *segment;
  RoadSegmentResourcePosition *resource;

  object->time_step = tick - object->last_update_tick;
  object->last_update_tick = tick;

  if(object->prepare_update != 0)
    {
      object->prepare_update(object);
    }

  if(object->perform_update != 0)
    {
      object->perform_update(object);

      segment = object->track_segment;
      if(segment != 0 &&
         (segment->previous_links[RACE_OBJECT_SEGMENT_SELF_LINK] ==
          segment ||
          segment->next_links[RACE_OBJECT_SEGMENT_SELF_LINK] ==
          segment))
        {
          resource = (RoadSegmentResourcePosition *)segment->resource_handle;
          if((resource->track_position >>
              RACE_OBJECT_TRACK_FRACTION_SHIFT) !=
             (object->position_z >> RACE_OBJECT_TRACK_FRACTION_SHIFT))
            {
              reset_race_object(object,
                                RACE_OBJECT_RESET_TRACK_MISMATCH);
            }
        }

      refresh_race_object_update_interval(object, tick);

      if(object->position_x < -RACE_OBJECT_LATERAL_POSITION_LIMIT ||
         object->position_x > RACE_OBJECT_LATERAL_POSITION_LIMIT)
        {
          reset_race_object(object, RACE_OBJECT_RESET_LATERAL_BOUNDS);
        }
    }

  object->next_update_tick = tick + object->update_interval_ticks;
  if((enforce_visibility_window & RACE_OBJECT_BOOLEAN_MASK) == 0)
    {
      return;
    }

  if(race_object_inside_active_window(object))
    {
      return;
    }

  reset_race_object(object, RACE_OBJECT_RESET_TRACK_MISMATCH);
}
