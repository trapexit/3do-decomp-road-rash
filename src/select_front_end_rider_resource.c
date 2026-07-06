#include "front_end_commands.h"
#include "front_end_transition_runtime.h"

FrontEndCelDescriptor *
select_front_end_rider_resource(FrontEndCelDescriptor *descriptors,
                                int                    base_resource_id)
{
  RiderProfile *active_profile;
  int active_player_index;
  int rider_type;
  int resource_offset;

  active_player_index = race_mode_state.active_player_index;
  active_profile = &player_profiles[active_player_index];
  rider_type = active_profile->rider_type;

  switch(rider_type)
    {
    case RIDER_TYPE_MIKE:
      resource_offset = FRONT_END_RIDER_MIKE_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_CYDNEY:
      resource_offset = FRONT_END_RIDER_CYDNEY_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_JIM:
      resource_offset = FRONT_END_RIDER_JIM_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_PEARL:
      resource_offset = FRONT_END_RIDER_PEARL_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_JON:
      resource_offset = FRONT_END_RIDER_JON_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_RHONDA:
      resource_offset = FRONT_END_RIDER_RHONDA_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_BOSE:
      resource_offset = FRONT_END_RIDER_BOSE_RESOURCE_OFFSET;
      break;
    case RIDER_TYPE_AXLE:
      resource_offset = FRONT_END_RIDER_AXLE_RESOURCE_OFFSET;
      break;
    default:
      return descriptors;
    }

  descriptors[FRONT_END_RIDER_DESCRIPTOR_INDEX].resource_id =
    base_resource_id + resource_offset;
  return &descriptors[FRONT_END_RIDER_DESCRIPTOR_INDEX];
}
