#include "stdlib.h"

#include "rider_animation_runtime.h"
#include "rw_semantic_data.h"

enum RacerObjectUpdateValue
{
  RACER_OBJECT_MODE_RECOVERY = 1,
  RACER_OBJECT_MODE_STOPPED = 2,
  RACER_DISABLED_ANIMATION_FIRST = 0x2C,
  RACER_DISABLED_ANIMATION_RANDOM_LOW = 0x2E,
  RACER_DISABLED_ANIMATION_RANDOM_HIGH = 0x2F,
  RACER_DISABLED_ANIMATION_IDLE = 0x30,
  RACER_DISABLED_ANIMATION_RANDOM_MASK = 1,
  RACER_TRACK_POSITION_FRACTION_BITS = 8,
  RACER_CONTACT_ANIMATION_TICK_LIMIT = 0x0C,
  RACER_CONTACT_ANIMATION_FRAME_SHIFT = 2
};

void
update_race_boost_charge(RacerEntity *rider);

int
racer_object_update(RacerEntity *rider)
{
  RiderAnimationRuntime *animation;
  RacerEntity *state;
  int rider_mode;
  int animation_state;
  int elapsed_ticks;
  int contact_animation_frame;

  animation = &rider->animation;
  state = rider->paired_state;

  if(rider == (RacerEntity *)race_rider_state.player_object)
    {
      update_race_boost_charge(rider);
    }

  rider_mode = rider->rider_mode;
  if(rider_mode == RACER_OBJECT_MODE_STOPPED ||
     rider_mode == RACER_OBJECT_MODE_RECOVERY)
    {
      if(rider->velocity.forward == 0)
        {
          select_primary_rider_animation(
            animation, RACER_DISABLED_ANIMATION_IDLE);
        }
      else
        {
          animation_state = (signed char)animation->primary_state;
          if(animation_state < RACER_DISABLED_ANIMATION_FIRST ||
             animation_state > RACER_DISABLED_ANIMATION_IDLE)
            {
              if((rand() & RACER_DISABLED_ANIMATION_RANDOM_MASK) == 0)
                {
                  animation_state = RACER_DISABLED_ANIMATION_RANDOM_HIGH;
                }
              else
                {
                  animation_state = RACER_DISABLED_ANIMATION_RANDOM_LOW;
                }
              select_primary_rider_animation(animation, animation_state);
              select_secondary_rider_animation(
                animation, RACER_DISABLED_ANIMATION_FIRST);
            }
        }

      return advance_car_animation(animation);
    }

  animation->current_frame = 0;
  if(state->pending_contact_animation_frame != 0)
    {
      if(state->last_contact_track_cell ==
         (rider->position_z >> RACER_TRACK_POSITION_FRACTION_BITS))
        {
          elapsed_ticks = rider->last_update_tick -
                          state->contact_frame_tick;
          contact_animation_frame = 0;
          if((unsigned int)elapsed_ticks <
             RACER_CONTACT_ANIMATION_TICK_LIMIT)
            {
              contact_animation_frame =
                ((unsigned int)elapsed_ticks >>
                 RACER_CONTACT_ANIMATION_FRAME_SHIFT) + 1;
            }
          state->pending_contact_animation_frame =
            (unsigned char)contact_animation_frame;
        }
      else
        {
          state->pending_contact_animation_frame = 0;
        }
    }

  contact_animation_frame = state->pending_contact_animation_frame;
  animation->primary_frame = (unsigned char)contact_animation_frame;
  return contact_animation_frame;
}
