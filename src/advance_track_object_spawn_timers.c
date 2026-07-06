#include "stdlib.h"

#include "division_helpers.h"
#include "rw_semantic_data.h"
#include "track_object_runtime.h"

enum TrackObjectSpawnWorkspaceIndex
{
  TRACK_OBJECT_REVERSE_SPEED_SCALE = 0,
  TRACK_OBJECT_REVERSE_SPAWN_PERIOD = 1,
  TRACK_OBJECT_REVERSE_SPAWN_TIMER = 2,
  TRACK_OBJECT_FORWARD_SPEED_SCALE = 3,
  TRACK_OBJECT_FORWARD_SPAWN_PERIOD = 4,
  TRACK_OBJECT_FORWARD_TRAILING_TIMER = 5,
  TRACK_OBJECT_FORWARD_LEADING_TIMER = 6,
  TRACK_OBJECT_SPAWN_PERIOD_LIMIT_MULTIPLIER = 2
};

#define TRACK_POSITION_TO_INTEGER(value)                                \
        (((value) + ((value) < 0 ? TRACK_OBJECT_POSITION_UNIT - 1 : 0)) >> \
         TRACK_OBJECT_POSITION_FRACTION_BITS)

#define ROAD_LANE_AT_TRACK_POSITION(position)                            \
        (gRoadSegmentEffectWorkspace.segment_index                           \
         .by_track_index[((unsigned int)(                                \
                            (position) >>                              \
                            TRACK_OBJECT_POSITION_FRACTION_BITS)) &    \
                         (ROAD_SEGMENT_COUNT - 1)]                        \
         ->selected_lanes[0])

void
advance_track_object_spawn_timers(int track_delta)
{
  UnsignedDivisionResult unsigned_division;
  SignedDivisionResult signed_division;
  RoadSegmentLaneRuntime *lane;
  int timer;
  int lane_count;

  timer = track_simulation_globals.active_workspace[
    TRACK_OBJECT_REVERSE_SPAWN_TIMER] -
          (track_delta + TRACK_POSITION_TO_INTEGER(
             track_simulation_globals.active_workspace[
               TRACK_OBJECT_REVERSE_SPEED_SCALE]));
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_REVERSE_SPAWN_TIMER] = timer;
  if(timer < 0)
    {
      lane = ROAD_LANE_AT_TRACK_POSITION(
        track_runtime_globals.right_bound - TRACK_OBJECT_POSITION_UNIT);
      lane_count = (signed char)TRACK_POSITION_TO_INTEGER(lane->left_width);
      if(lane_count > 0)
        {
          unsigned_division = unsigned_divide_with_remainder(
            (unsigned int)lane_count, (unsigned int)rand());
          create_track_object(
            TRACK_OBJECT_MODE_REVERSE,
            (signed char)(unsigned_division.remainder - lane_count),
            0, 0, lane);
        }
      track_simulation_globals.active_workspace[
        TRACK_OBJECT_REVERSE_SPAWN_TIMER] +=
        track_simulation_globals.active_workspace[
          TRACK_OBJECT_REVERSE_SPAWN_PERIOD];
    }

  timer = track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_TRAILING_TIMER] -
          track_delta;
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_TRAILING_TIMER] = timer;
  timer += TRACK_POSITION_TO_INTEGER(
    track_simulation_globals.active_workspace[
      TRACK_OBJECT_FORWARD_SPEED_SCALE]);
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_TRAILING_TIMER] = timer;
  if(timer < 0)
    {
      lane = ROAD_LANE_AT_TRACK_POSITION(
        track_runtime_globals.right_bound - TRACK_OBJECT_POSITION_UNIT);
      lane_count = (signed char)TRACK_POSITION_TO_INTEGER(lane->right_width);
      if(lane_count > 0)
        {
          signed_division = signed_divide_with_remainder(
            (unsigned int)lane_count, (unsigned int)rand());
          create_track_object(
            TRACK_OBJECT_MODE_FORWARD,
            (signed char)signed_division.remainder, 0, 0, lane);
        }
      track_simulation_globals.active_workspace[
        TRACK_OBJECT_FORWARD_TRAILING_TIMER] +=
        track_simulation_globals.active_workspace[
          TRACK_OBJECT_FORWARD_SPAWN_PERIOD];
    }
  else if(track_simulation_globals.active_workspace[
            TRACK_OBJECT_FORWARD_SPAWN_PERIOD] *
          TRACK_OBJECT_SPAWN_PERIOD_LIMIT_MULTIPLIER < timer)
    {
      track_simulation_globals.active_workspace[
        TRACK_OBJECT_FORWARD_TRAILING_TIMER] =
        track_simulation_globals.active_workspace[
          TRACK_OBJECT_FORWARD_SPAWN_PERIOD] << 1;
    }

  timer = track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_LEADING_TIMER] -
          track_delta;
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_LEADING_TIMER] = timer;
  timer += TRACK_POSITION_TO_INTEGER(
    track_simulation_globals.active_workspace[
      TRACK_OBJECT_FORWARD_SPEED_SCALE]);
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_LEADING_TIMER] = timer;
  if(timer <= 0)
    {
      timer = -(track_simulation_globals.active_workspace[
                  TRACK_OBJECT_FORWARD_SPAWN_PERIOD] << 1);
      if(timer > track_simulation_globals.active_workspace[
           TRACK_OBJECT_FORWARD_LEADING_TIMER])
        {
          track_simulation_globals.active_workspace[
            TRACK_OBJECT_FORWARD_LEADING_TIMER] = timer;
        }
      return;
    }

  lane = ROAD_LANE_AT_TRACK_POSITION(
    track_runtime_globals.left_bound + TRACK_OBJECT_POSITION_UNIT);
  lane_count = (signed char)TRACK_POSITION_TO_INTEGER(lane->right_width);
  if(lane_count > 0)
    {
      signed_division = signed_divide_with_remainder(
        (unsigned int)lane_count, (unsigned int)rand());
      create_track_object(
        TRACK_OBJECT_MODE_FORWARD,
        (signed char)signed_division.remainder, 0, 0, lane);
    }
  track_simulation_globals.active_workspace[
    TRACK_OBJECT_FORWARD_LEADING_TIMER] -=
    track_simulation_globals.active_workspace[
      TRACK_OBJECT_FORWARD_SPAWN_PERIOD];
}


#undef ROAD_LANE_AT_TRACK_POSITION
#undef TRACK_POSITION_TO_INTEGER
