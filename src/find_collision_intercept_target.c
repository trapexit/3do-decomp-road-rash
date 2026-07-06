#include "ai_collision_avoidance_runtime.h"
#include "division_helpers.h"

TrackInteractionObject *
find_track_object_in_direction(TrackInteractionObject *object,
                               int                     mask,
                               int                     mode,
                               int                     maximum_distance);

enum CollisionInterceptSearchValue
{
  COLLISION_INTERCEPT_SIGN_SHIFT = 31,
  COLLISION_INTERCEPT_POSITION_FRACTION_BITS = 8
};

#define COLLISION_INTERCEPT_MOVING_FLAGS 0x3CU

static
int
collision_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >>
                COLLISION_INTERCEPT_SIGN_SHIFT)) >> 1;
}


TrackInteractionObject *
find_collision_intercept_target(TrackInteractionObject *origin,
                                TrackInteractionObject *excluded,
                                int                     object_mask,
                                int                     search_mode,
                                int                     maximum_intercept_time,
                                int                     maximum_distance,
                                int                     lateral_window_width,
                                int                    *intercept_time)
{
  TrackInteractionObject *candidate;
  TrackInteractionObject *best;
  SignedDivisionResult division;
  int window_low;
  int window_high;
  int best_score;
  int candidate_width;
  int candidate_low;
  int candidate_high;
  int direction;
  int delta;
  int score;

  best = 0;
  window_low = origin->position.lateral_position -
               collision_half_towards_zero(lateral_window_width);
  window_high = window_low + lateral_window_width;
  best_score = maximum_intercept_time + 1;

  candidate = find_track_object_in_direction(
    origin, object_mask, search_mode, maximum_distance);
  while(candidate != 0)
    {
      if(candidate != excluded)
        {
          candidate_width =
            collision_avoidance_lateral_width(candidate);
          candidate_low = candidate->position.lateral_position -
                          collision_half_towards_zero(
            candidate_width);
          candidate_high = candidate_low + candidate_width;
          if(candidate_high <= window_low)
            {
              return best;
            }
          if(window_high <= candidate_low)
            {
              return best;
            }

          if((candidate->collision_flags &
              COLLISION_INTERCEPT_MOVING_FLAGS) != 0)
            {
              direction = origin->world_velocity.forward -
                          candidate->world_velocity.forward;
            }
          else
            {
              direction = origin->world_velocity.forward;
            }

          if(direction == 0)
            {
              goto next_candidate;
            }
          if(best != 0)
            {
              if(direction > 0)
                {
                  if(candidate->position.track_position <=
                     origin->position.track_position)
                    {
                      goto next_candidate;
                    }
                }
              else if(direction < 0)
                {
                  if(candidate->position.track_position >=
                     origin->position.track_position)
                    {
                      goto next_candidate;
                    }
                }
            }

          delta = origin->position.track_position -
                  candidate->position.track_position;
          if(delta <= 0)
            {
              delta = -delta;
            }
          if(direction <= 0)
            {
              direction = -direction;
            }

          division = signed_divide_with_remainder(
            direction,
            delta << COLLISION_INTERCEPT_POSITION_FRACTION_BITS);
          score = division.quotient;
          if(score <= maximum_intercept_time &&
             score < best_score)
            {
              *intercept_time = score;
              best_score = score;
              best = candidate;
            }
        }

next_candidate:
      candidate = find_track_object_in_direction(
        candidate, object_mask, search_mode, maximum_distance);
    }

  return best;
}


#undef COLLISION_INTERCEPT_MOVING_FLAGS
