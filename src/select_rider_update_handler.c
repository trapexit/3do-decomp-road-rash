#include "division_helpers.h"
#include "rider_contact_runtime.h"
#include "track_traversal_runtime.h"

enum RiderUpdateSelectionConstant
{
  RIDER_CONTROL_MODE_STOPPED = 2,
  RIDER_CONTROL_MODE_FINISHING = 3,
  RIDER_TARGET_CLASS_LOW = 1,
  RIDER_TARGET_CLASS_MEDIUM = 2,
  RIDER_TARGET_CLASS_HIGH = 3,
  RIDER_UPDATE_HANDLER_CRUISE = 1,
  RIDER_UPDATE_HANDLER_PASS_RIGHT = 2,
  RIDER_UPDATE_HANDLER_ATTACK = 3,
  RIDER_UPDATE_HANDLER_APPROACH = 4,
  RIDER_UPDATE_HANDLER_PASS_LEFT = 5,
  RIDER_UPDATE_HANDLER_FINISH = 8,
  RIDER_UPDATE_HANDLER_INACTIVE = 9,
  RIDER_LANE_WIDTH_DIVISOR = 0xFA00
};

static
int
divide_lane_offset(int value)
{
  SignedDivisionResult division;

  division = signed_divide_with_remainder(
    RIDER_LANE_WIDTH_DIVISOR, (unsigned int)value);
  return division.quotient;
}


static
int
is_inside_lane_right_boundary(int offset,
                              int right_width)
{
  int lane_units;

  lane_units = divide_lane_offset(offset);
  if(offset < 0)
    {
      lane_units--;
    }
  if(lane_units < 0)
    {
      return 0;
    }

  if(right_width < 0)
    {
      right_width += TRACK_POSITION_FRACTION_MASK;
    }
  return lane_units <
         (right_width >> TRACK_POSITION_FRACTION_BITS);
}


void
select_rider_update_handler(RacerEntity *rider)
{
  RiderInteractionRuntime *interaction;
  RacerEntity *target;
  RoadSegmentLaneRuntime *lane;
  int target_class;
  int lane_offset;

  interaction = rider->paired_state->interaction_data;
  target = interaction->attack_target;

  if(rider->rider_mode != 0)
    {
      if(rider->rider_mode == RIDER_CONTROL_MODE_STOPPED ||
         rider->rider_mode == RIDER_CONTROL_MODE_FINISHING)
        {
          set_rider_update_handler(
            rider, RIDER_UPDATE_HANDLER_FINISH, 0);
        }
      else
        {
          set_rider_update_handler(
            rider, RIDER_UPDATE_HANDLER_INACTIVE, 0);
        }
      return;
    }

  if(target == 0 || target->track_segment == 0)
    {
      if(interaction->handler_index != RIDER_UPDATE_HANDLER_CRUISE)
        {
          set_rider_update_handler(
            rider, RIDER_UPDATE_HANDLER_CRUISE, 0);
        }
      return;
    }

  if(is_rider_attack_animation(rider) != false)
    {
      return;
    }

  target_class = interaction->attack_target_class;
  if(target_class == RIDER_TARGET_CLASS_LOW)
    {
      set_rider_update_handler(
        rider,
        is_rider_left_of_target(rider, target) == false
                ? RIDER_UPDATE_HANDLER_CRUISE
                : RIDER_UPDATE_HANDLER_PASS_RIGHT,
        target);
      return;
    }
  if(target_class == RIDER_TARGET_CLASS_HIGH)
    {
      set_rider_update_handler(
        rider,
        can_rider_start_attack(rider, target) == false
                ? RIDER_UPDATE_HANDLER_APPROACH
                : RIDER_UPDATE_HANDLER_ATTACK,
        target);
      return;
    }
  if(target_class != RIDER_TARGET_CLASS_MEDIUM)
    {
      return;
    }

  lane = rider->track_segment;
  lane_offset = rider->position_x -
                (lane->center_position << TRACK_POSITION_FRACTION_BITS);
  if(is_inside_lane_right_boundary(
       lane_offset, lane->right_width) != 0 &&
     can_rider_start_attack(rider, target) != false)
    {
      set_rider_update_handler(
        rider, RIDER_UPDATE_HANDLER_ATTACK, target);
      return;
    }

  lane = target->track_segment;
  lane_offset = target->position_x -
                (lane->center_position << TRACK_POSITION_FRACTION_BITS);
  if(is_inside_lane_right_boundary(
       lane_offset, lane->right_width) != 0 &&
     is_rider_close_on_left(rider, target) != false)
    {
      set_rider_update_handler(
        rider, RIDER_UPDATE_HANDLER_PASS_LEFT, target);
      return;
    }

  set_rider_update_handler(
    rider, RIDER_UPDATE_HANDLER_CRUISE, target);
}
