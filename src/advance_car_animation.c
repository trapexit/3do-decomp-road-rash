#include "rider_animation_runtime.h"

enum RiderAnimationTransitionValue
{
  RIDER_ANIMATION_UNAVAILABLE_STATE = -1,
  RIDER_ANIMATION_DEFAULT_STATE = 0,
  RIDER_ANIMATION_ADVANCE_COMMAND = 0,
  RIDER_ANIMATION_LOOP_COMMAND = 1,
  RIDER_ANIMATION_COMMAND_MASK = 0x0F,
  RIDER_ANIMATION_LOOP_FRAME_MASK = 0xF0,
  RIDER_ANIMATION_LOOP_FRAME_SHIFT = 4
};

int
advance_car_animation(RiderAnimationRuntime *animation)
{
  CelAnimationBinding *binding;
  CelAnimationBinding **state_table;
  int state;
  int frame;
  int next_frame_tick;
  int command;
  int transition;

  state = (signed char)animation->primary_state;
  if(state == RIDER_ANIMATION_UNAVAILABLE_STATE)
    {
      state = RIDER_ANIMATION_DEFAULT_STATE;
    }
  state = (signed char)state;

  state_table = animation->state_table;
  if(state_table == 0)
    {
      animation->current_frame = 0;
      return (int)animation;
    }
  binding = state_table[state];
  if(binding == 0)
    {
      animation->current_frame = 0;
      return (int)animation;
    }
  frame = (signed char)animation->primary_frame;
  binding += frame;
  animation->current_frame = binding;

  next_frame_tick = animation->next_frame_tick;
  if(next_frame_tick == -1)
    {
      next_frame_tick = binding->duration_ticks + frame_tick;
      animation->next_frame_tick = next_frame_tick;
    }

  if(frame_tick < next_frame_tick)
    {
      return (int)animation;
    }

  transition = (signed char)binding->transition_command;
  command = transition & RIDER_ANIMATION_COMMAND_MASK;
  if(command == RIDER_ANIMATION_ADVANCE_COMMAND)
    {
      animation->primary_frame++;
    }
  else if(command == RIDER_ANIMATION_LOOP_COMMAND &&
          (signed char)animation->secondary_state ==
          RIDER_ANIMATION_UNAVAILABLE_STATE)
    {
      animation->primary_frame = (unsigned char)(
        (transition & RIDER_ANIMATION_LOOP_FRAME_MASK) >>
        RIDER_ANIMATION_LOOP_FRAME_SHIFT);
      animation->cycle_count++;
    }
  else
    {
      animation->primary_state = animation->secondary_state;
      animation->primary_frame = 0;
      animation->cycle_count = 0;
      animation->secondary_state =
        (unsigned char)RIDER_ANIMATION_UNAVAILABLE_STATE;
      state = (signed char)animation->primary_state;
      if(state == RIDER_ANIMATION_UNAVAILABLE_STATE)
        {
          state = RIDER_ANIMATION_DEFAULT_STATE;
        }
      state = (signed char)state;
    }

  state_table = animation->state_table;
  if(state_table == 0)
    {
      animation->current_frame = 0;
      return (int)animation;
    }
  binding = state_table[state];
  if(binding == 0)
    {
      animation->current_frame = 0;
      return (int)animation;
    }
  frame = (signed char)animation->primary_frame;
  binding += frame;

  next_frame_tick = animation->next_frame_tick + binding->duration_ticks;
  animation->next_frame_tick = next_frame_tick;
  return next_frame_tick;
}
