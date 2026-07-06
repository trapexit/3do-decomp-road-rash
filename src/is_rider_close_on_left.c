#include "stdlib.h"
/* Reconstructed from the original function at 0x000063CC using Ghidra and assembly. */

#include "event.h"

#include "bss_late_data.h"
#include "racer_runtime.h"
#include "rider_animation_runtime.h"
#include "rw_semantic_data.h"

/* Axis note: 0x1C is RacerEntity.position_z and 0x28 is position_x; the
 * old LATERAL_/TRACK_ names below disagreed across units about which is
 * which. Converted code uses struct fields; axis English needs runtime
 * proof before any rename. */

#define INPUT_STATE (track_simulation_globals.control_bits)

enum
{
  DIVIDE_BY_FOUR_ROUNDING_BIAS = 3,
  DIVIDE_BY_FOUR_SHIFT = 2,
  DIVIDE_BY_256_ROUNDING_BIAS = 0xFF,
  RIDER_TRACK_POSITION_FRACTION_BITS = 8,
  RIDER_LEFT_CLEARANCE_LIMIT = 0xF0,
  RIDER_TRACK_PROXIMITY_LIMIT = 0x8000,
  CHALLENGE_TRACK_PROXIMITY_LIMIT = 0x9000,
  RIDER_INTERACTION_READY_PROGRESS = 0x4B0,
  RIDER_INTERACTION_VERTICAL_OFFSET = 0x1770,
  RIDER_INTERACTION_HORIZONTAL_TOLERANCE = 9,
  RIDER_INTERACTION_DISTANCE_LIMIT = 0x50,
  RIDER_ANGLE_INPUT_STEP = 0x100,
  FIXED_ANGLE_HALF_TURN = 0x00800000,
  RIDER_EVENT_FIGHT = 5,
  RIDER_EVENT_DURATION = 0xB4,
  RIDER_PRIMARY_ANIMATION_WAIT = 0x25,
  RIDER_PRIMARY_ANIMATION_ALIGN = 0x26,
  RIDER_PRIMARY_ANIMATION_FINISH = 0x29,
  RIDER_SECONDARY_ANIMATION_FINISH = 2,
  RIDER_INTERACTION_MODE_ALIGN = 2,
  RIDER_INTERACTION_MODE_FINISH = 3,
  TRACK_SEGMENT_ROAD_KIND = 1
};

/* The pad bits the walk reads.  The previous names were rotated one position
   from the bits they held (0x80000000 is ControlDown, not ControlLeft, and so
   on), which reads as left/right walking along the track and up/down moving
   sideways.  The walk steers with the same pad orientation as the bike:
   up/down travel along the track, left/right move across it. */
#define RIDER_DIRECTION_INPUT_MASK \
        (ControlDown | ControlUp | ControlRight | ControlLeft)
#define RIDER_STATE_CONTROL_DISABLED 0x10U
#define FIXED_ANGLE_MASK 0x00FFFFFFU
#define FIXED_ANGLE_SIGN_EXTENSION 0xFF000000U

typedef struct AiRacerObject AiRacerObject;

int
select_ai_collision_avoidance_target(AiRacerObject *rider);
int
select_ai_lane_target(AiRacerObject *rider);
int
apply_ai_rider_navigation_control(RacerEntity *rider);
int
rider_update_mode_1(int rider);
int
schedule_race_outcome_event(int event,
                            int value);
int
fixed_vector_angle_24(int horizontal,
                      int vertical);
int
absolute_int_difference(int first_position,
                        int second_position);

static
int
div4_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_FOUR_ROUNDING_BIAS;
    }
  return value >> DIVIDE_BY_FOUR_SHIFT;
}


static
int
div256_toward_zero(int value)
{
  if(value < 0)
    {
      value += DIVIDE_BY_256_ROUNDING_BIAS;
    }
  return value >> RIDER_TRACK_POSITION_FRACTION_BITS;
}


static
int
track_position_outside(char *object)
{
  RacerEntity *racer;
  RoadSegmentLaneRuntime *track;
  int position;

  racer = (RacerEntity *)object;
  track = racer->primary_track;
  position = racer->position_x >>
             RIDER_TRACK_POSITION_FRACTION_BITS;
  return position < track->edge_shapes[0].inner_offset ||
         position > track->edge_shapes[1].inner_offset;
}


int
is_rider_close_on_left(char *right_rider,
                       char *left_rider)
{
  RacerEntity *right;
  RacerEntity *left;
  int edge0;
  int edge1;
  int distance;

  right = (RacerEntity *)right_rider;
  left = (RacerEntity *)left_rider;

  edge0 = right->position_z - right->collision_half_width_x;
  edge1 = left->position_z + left->collision_half_width_x;
  if(edge1 >= edge0)
    {
      return 0;
    }

  distance = edge0 - edge1;
  if(distance >= RIDER_LEFT_CLEARANCE_LIMIT)
    {
      return 0;
    }

  distance = absolute_int_difference(
    right->position_x - left->position_x,
    0);
  return (unsigned char)(distance < RIDER_TRACK_PROXIMITY_LIMIT);
}


int
rider_update_mode_5(char *rider)
{
  RacerEntity *rider_entity;
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RacerEntity *target;
  RacerEntity *current;
  int target_y;
  int distance;

  rider_entity = (RacerEntity *)rider;
  state = rider_entity->paired_state;
  interaction = state->interaction_data;

  if(select_ai_collision_avoidance_target(
       (AiRacerObject *)rider) == 0)
    {
      select_ai_lane_target((AiRacerObject *)rider);
      target = interaction->attack_target;
      target_y = target->position_x;
      distance = absolute_int_difference(
        interaction->target_lateral_position - target_y,
        0);

      if(distance >= CHALLENGE_TRACK_PROXIMITY_LIMIT)
        {
          current = (RacerEntity *)race_rider_state.challenge_object;
          if(current == 0 ||
             state != current->paired_state)
            {
              return rider_update_mode_1((int)rider);
            }
        }
      interaction->target_lateral_position = target_y;
    }

  return apply_ai_rider_navigation_control((RacerEntity *)rider);
}


int
rider_update_mode_6(char *rider)
{
  RacerEntity *rider_entity;
  RiderInteractionRuntime *interaction;
  RacerEntity *target;

  rider_entity = (RacerEntity *)rider;
  interaction = rider_entity->paired_state->interaction_data;
  if(select_ai_collision_avoidance_target(
       (AiRacerObject *)rider) == 0)
    {
      target = interaction->attack_target;
      interaction->target_speed = interaction->target_speed_limit;
      interaction->target_track_position = target->position_z;
      interaction->target_lateral_position = target->position_x;
    }
  return apply_ai_rider_navigation_control((RacerEntity *)rider);
}


int
rider_update_mode_7(char *rider)
{
  RacerEntity *rider_entity;
  RiderInteractionRuntime *interaction;
  RacerEntity *target;

  rider_entity = (RacerEntity *)rider;
  interaction = rider_entity->paired_state->interaction_data;
  if(select_ai_collision_avoidance_target(
       (AiRacerObject *)rider) == 0)
    {
      target = interaction->attack_target;
      interaction->target_track_position = target->position_z;
      interaction->target_lateral_position = target->position_x;
    }
  return apply_ai_rider_navigation_control((RacerEntity *)rider);
}


int
rider_update_mode_8(char *rider)
{
  RacerEntity *rider_entity;
  RacerEntity *state;
  RiderInteractionRuntime *interaction;
  RiderAnimationRuntime *animation;
  RacerEntity *current;
  int mode;
  int animation_state;
  int animation_count;
  int index;
  int value;
  int horizontal;
  int vertical;
  int result;
  int action;
  unsigned int input;

  rider_entity = (RacerEntity *)rider;
  state = rider_entity->paired_state;
  interaction = state->interaction_data;
  animation = &state->animation;
  mode = rider_entity->rider_mode;

  if(mode == RIDER_INTERACTION_MODE_FINISH)
    {
      animation_state = animation->primary_state;
      if(animation_state != RIDER_PRIMARY_ANIMATION_FINISH)
        {
          /* reset takes no state arg and returns void; the old prototype
           * claimed otherwise and this return value is discarded by the
           * handler dispatch. */
          reset_rider_to_normal_mode(rider_entity);
          return 0;
        }
      return (int)rider;
    }
  if(mode != RIDER_INTERACTION_MODE_ALIGN)
    {
      return (int)rider;
    }

  index = (signed char)interaction->handler_index;
  value = interaction->handler_elapsed[index];
  if(value > RIDER_INTERACTION_READY_PROGRESS)
    {
      current = (RacerEntity *)race_rider_state.challenge_object;
      if(current == 0 ||
         state != current->paired_state)
        {
          action = rider_entity->current_bike_health;
          if(action == 0)
            {
              if(rider == (char *)race_rider_state.player_object)
                {
                  schedule_race_outcome_event(RIDER_EVENT_FIGHT, RIDER_EVENT_DURATION);
                }
              state->velocity.forward = 0;
              if(animation->primary_state !=
                 RIDER_PRIMARY_ANIMATION_WAIT)
                {
                  select_primary_rider_animation(animation, RIDER_PRIMARY_ANIMATION_WAIT);
                }
            }
          else
            {
              select_primary_rider_animation(animation, RIDER_PRIMARY_ANIMATION_FINISH);
              result = select_secondary_rider_animation(
                animation, RIDER_SECONDARY_ANIMATION_FINISH);
              rider_entity->rider_mode =
                RIDER_INTERACTION_MODE_FINISH;
              return result;
            }
        }
    }

  animation_state = animation->primary_state;
  if(animation_state == RIDER_PRIMARY_ANIMATION_ALIGN)
    {
      if(rider == (char *)race_rider_state.player_object &&
         (state->status_flags &
          RIDER_STATE_CONTROL_DISABLED) == 0 &&
         rider_entity->primary_track->geometry_mode ==
         TRACK_SEGMENT_ROAD_KIND)
        {
          if(track_position_outside(rider) ||
             track_position_outside((char *)state))
            {
              result =
                schedule_race_outcome_event(RIDER_EVENT_FIGHT, RIDER_EVENT_DURATION);
              rider_entity->current_bike_health = 0;
              return result;
            }
        }

      horizontal = rider_entity->position_z -
                   state->position_z;
      vertical = rider_entity->position_x -
                 RIDER_INTERACTION_VERTICAL_OFFSET -
                 state->position_x;

      if(absolute_int_difference(horizontal, 0) <=
           RIDER_INTERACTION_HORIZONTAL_TOLERANCE &&
         absolute_int_difference(vertical, 0) <=
           RIDER_INTERACTION_VERTICAL_OFFSET)
        {
          if(rider_entity->current_bike_health == 0)
            {
              state->velocity.forward = 0;
              return select_primary_rider_animation(animation,
                                                    RIDER_PRIMARY_ANIMATION_WAIT);
            }

          value = state->position_x +
                  RIDER_INTERACTION_VERTICAL_OFFSET;
          state->position_x = value;
          rider_entity->position_x = value;
          state->velocity.forward = 0;
          select_primary_rider_animation(animation, RIDER_PRIMARY_ANIMATION_FINISH);
          result = select_secondary_rider_animation(animation,
                                                    RIDER_SECONDARY_ANIMATION_FINISH);
          rider_entity->rider_mode =
            RIDER_INTERACTION_MODE_FINISH;
          return result;
        }

      if(rider == (char *)race_rider_state.player_object)
        {
          input = INPUT_STATE;
          if((input & RIDER_DIRECTION_INPUT_MASK) != 0)
            {
              if((input & ControlDown) != 0 && horizontal < 0)
                {
                  horizontal = -1;
                }
              else if((input & ControlUp) != 0 &&
                      horizontal > 0)
                {
                  horizontal = 1;
                }
              else
                {
                  horizontal = 0;
                }

              if((input & ControlLeft) != 0)
                {
                  vertical = -RIDER_ANGLE_INPUT_STEP;
                }
              else if((input & ControlRight) != 0)
                {
                  vertical = RIDER_ANGLE_INPUT_STEP;
                }
              else
                {
                  vertical = 0;
                }

              index = (signed char)interaction->handler_index;
              interaction->handler_elapsed[index] = 0;
            }
        }

      result = fixed_vector_angle_24(horizontal,
                                     div256_toward_zero(vertical));
      result &= FIXED_ANGLE_MASK;
      state->heading = result;
      if((unsigned int)result > FIXED_ANGLE_HALF_TURN)
        {
          result += (int)FIXED_ANGLE_SIGN_EXTENSION;
          state->heading = result;
        }
      state->velocity.forward =
        RIDER_INTERACTION_READY_PROGRESS;
      /* The walk is drawn with the angle it travels at: the render snapshot
         copies orientation into previous_orientation every frame, and the
         CANS orientation takes the sprite's tilt from it. */
      state->orientation.movement_heading = state->heading;
      return state->heading;
    }

  if(animation_state != RIDER_PRIMARY_ANIMATION_WAIT)
    {
      return animation_state;
    }

  if(rider_entity->current_bike_health == 0 &&
     absolute_int_difference(state->position_z,
                             rider_entity->position_z) <
     RIDER_INTERACTION_DISTANCE_LIMIT)
    {
      animation_count = animation->cycle_count;
      if(animation_count < 1)
        {
          return animation_count;
        }

      if(rider == (char *)race_rider_state.player_object &&
         (state->status_flags &
          RIDER_STATE_CONTROL_DISABLED) == 0)
        {
          return schedule_race_outcome_event(RIDER_EVENT_FIGHT, RIDER_EVENT_DURATION);
        }

      value = div4_toward_zero(
        rider_entity->maximum_bike_health);
      rider_entity->current_bike_health = value;
      return value;
    }

  animation_count = animation->cycle_count;
  if(animation_count >= RIDER_SECONDARY_ANIMATION_FINISH)
    {
      return select_secondary_rider_animation(animation, RIDER_PRIMARY_ANIMATION_ALIGN);
    }
  return animation_count;
}


#undef INPUT_STATE
