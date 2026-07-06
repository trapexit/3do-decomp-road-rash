#include "division_helpers.h"
#include "rider_visual_runtime.h"
#include "rw_semantic_data.h"
#include "track_object_query.h"

enum RiderLaneChangeClearValue
{
  RIDER_LANE_CHANGE_NEGATIVE_SEARCH_MODE = 0,
  RIDER_LANE_CHANGE_POSITIVE_SEARCH_MODE = 2,
  RIDER_LANE_CHANGE_COLLISION_MASK = 0x14,
  RIDER_LANE_CHANGE_DISTANCE_SHIFT = 4,
  RIDER_LANE_CHANGE_DIRECTION_SCALE = 0x191,
  RIDER_LANE_CHANGE_SCALE_SHIFT = 8,
  RIDER_LANE_CHANGE_BOOLEAN_MASK = 0xFF
};

boolean
is_rider_lane_change_clear(const RiderVisualObject *rider,
                           int                      direction,
                           int                      lane_offset)
{
  TrackInteractionObject *candidate;
  SignedDivisionResult division;
  int lane;
  int search_mode;
  int distance;
  int candidate_speed;
  int absolute_speed;
  int threshold;
  int scale;
  int divisor;

  lane = compute_track_lane_index(rider, 0) + lane_offset;
  if(lane < 0)
    {
      search_mode = RIDER_LANE_CHANGE_NEGATIVE_SEARCH_MODE;
    }
  else
    {
      search_mode = RIDER_LANE_CHANGE_POSITIVE_SEARCH_MODE;
    }

  candidate = find_track_object_in_lane(
    (TrackInteractionObject *)(void *)rider, lane, search_mode,
    RIDER_LANE_CHANGE_COLLISION_MASK, -1);
  if(candidate == 0)
    {
      return true;
    }

  distance = rider->position.track_position -
             candidate->position.track_position;
  if(distance <= 0)
    {
      distance = -distance;
    }
  distance >>= RIDER_LANE_CHANGE_DISTANCE_SHIFT;

  scale = ((unsigned int)(direction * RIDER_LANE_CHANGE_DIRECTION_SCALE)) >>
          RIDER_LANE_CHANGE_SCALE_SHIFT;

  candidate_speed = candidate->world_velocity.forward;
  if(candidate_speed <= 0)
    {
      absolute_speed = -candidate_speed;
    }
  else
    {
      absolute_speed = candidate_speed;
    }

  threshold =
    (absolute_speed >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_A) +
    (absolute_speed >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_B) +
    (absolute_speed >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_C);
  divisor = (int)track_simulation_globals.packed_divisor + 1;
  division = signed_divide_with_remainder(
    (unsigned int)divisor,
    (unsigned int)(threshold << RIDER_LANE_CHANGE_SCALE_SHIFT));
  threshold = (scale * (int)division.quotient) >>
              RIDER_LANE_CHANGE_SCALE_SHIFT;

  return (threshold <= distance) & RIDER_LANE_CHANGE_BOOLEAN_MASK;
}
