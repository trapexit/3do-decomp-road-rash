#include "stdlib.h"

#include "division_helpers.h"
#include "front_end_cel_runtime.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"

enum FrontEndRandomPreviewValue
{
  FRONT_END_RANDOM_VARIANT_COUNT = 6,
  FRONT_END_RANDOM_RIDER_RESOURCE_BASE = 0x1D,
  FRONT_END_GAME_MODE_DESCRIPTOR_COUNT = 14,
  FRONT_END_RIDER_TYPE_DESCRIPTOR_COUNT = 8,
  FRONT_END_PODIUM_FINISH_MAXIMUM = 2,
  FRONT_END_MAXIMUM_PROGRESSION_LEVEL = 4,
  FRONT_END_OUTCOME_PODIUM = 2,
  FRONT_END_OUTCOME_NON_PODIUM = 3,
  FRONT_END_OUTCOME_BIKE_REPAIR = 4,
  FRONT_END_OUTCOME_FORFEIT = 5,
  FRONT_END_OUTCOME_ADVANCE_LEVEL = 0x7E,
  FRONT_END_PODIUM_RESOURCE_BASE = 0x378,
  FRONT_END_NON_PODIUM_RESOURCE_BASE = 0x37E,
  FRONT_END_BIKE_REPAIR_RESOURCE_BASE = 0x384,
  FRONT_END_FORFEIT_RESOURCE_BASE = 0x393,
  FRONT_END_MIKE_FINISH_RESOURCE_BASE = 0x3E0,
  FRONT_END_CYDNEY_FINISH_RESOURCE_BASE = 0x3DA,
  FRONT_END_JIM_FINISH_RESOURCE_BASE = 0x3E9,
  FRONT_END_PEARL_FINISH_RESOURCE_BASE = 0x3E3,
  FRONT_END_JON_FINISH_RESOURCE_BASE = 0x3DD,
  FRONT_END_RHONDA_FINISH_RESOURCE_BASE = 0x3E6,
  FRONT_END_BOSE_FINISH_RESOURCE_BASE = 0x3D7,
  FRONT_END_AXLE_FINISH_RESOURCE_BASE = 0x3D4
};

int
draw_front_end_cel_descriptor(const FrontEndCelDescriptor *descriptor);
int
start_front_end_menu_audio(void);
int
start_next_background_audio_stream(void);

int
front_end_choose_random_rider_preview(int operation,
                                      int selection)
{
  SignedDivisionResult random_variant;

  (void)operation;
  (void)selection;

  random_variant = signed_divide_with_remainder(
    FRONT_END_RANDOM_VARIANT_COUNT, (unsigned int)rand());
  front_end_cel_group_046[0].resource_id =
    (int)random_variant.remainder +
    FRONT_END_RANDOM_RIDER_RESOURCE_BASE;
  return 1;
}


int
front_end_show_profile_slot_descriptor(void)
{
  FrontEndCelDescriptor *descriptor;

  if(front_end_preview_state.profile_operation_state == 0)
    {
      descriptor = &profile_save_descriptor;
    }
  else
    {
      descriptor = &profile_load_descriptor;
    }
  return draw_front_end_cel_descriptor(descriptor);
}


int
front_end_show_game_mode_descriptor(void)
{
  static FrontEndCelDescriptor *const
  descriptors[FRONT_END_GAME_MODE_DESCRIPTOR_COUNT] =
  {
    front_end_cel_group_064, front_end_cel_group_065,
    front_end_cel_group_066, front_end_cel_group_067,
    front_end_cel_group_068, front_end_cel_group_069,
    front_end_cel_group_070, front_end_cel_group_071,
    front_end_cel_group_072, front_end_cel_group_073,
    front_end_cel_group_074, front_end_cel_group_075,
    front_end_cel_group_076, front_end_cel_group_077
  };
  int mode;

  mode = race_mode_state.previous_background_audio_index;
  if((unsigned int)mode >= FRONT_END_GAME_MODE_DESCRIPTOR_COUNT)
    {
      return mode;
    }
  return apply_front_end_cel_descriptors(descriptors[mode]);
}


int
front_end_show_rider_type_descriptor(void)
{
  static FrontEndCelDescriptor *const
  descriptors[FRONT_END_RIDER_TYPE_DESCRIPTOR_COUNT] =
  {
    front_end_cel_group_268, front_end_cel_group_263,
    front_end_cel_group_267, front_end_cel_group_265,
    front_end_cel_group_264, front_end_cel_group_266,
    front_end_cel_group_262, front_end_cel_group_261
  };
  int type_index;

  type_index =
    player_profiles[race_mode_state.active_player_index].rider_type - 1;
  if((unsigned int)type_index >= FRONT_END_RIDER_TYPE_DESCRIPTOR_COUNT)
    {
      return type_index;
    }
  return apply_front_end_cel_descriptors(descriptors[type_index]);
}


int
front_end_is_profile_slot_available(void)
{
  return front_end_preview_state.profile_operation_state <= 0;
}


int
front_end_show_finish_result_descriptor(void)
{
  FrontEndCelDescriptor descriptor;
  RiderProfile *profile;
  int variant;

  descriptor.resource_id = -1;
  descriptor.parent_index = -1;
  descriptor.child_selector = 0;

  variant = race_mode_state.finish_position;
  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD &&
     variant <= FRONT_END_PODIUM_FINISH_MAXIMUM)
    {
      profile = &player_profiles[race_mode_state.active_player_index];
      switch(profile->rider_type)
        {
        case RIDER_TYPE_MIKE:
          descriptor.resource_id =
            variant + FRONT_END_MIKE_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_CYDNEY:
          descriptor.resource_id =
            variant + FRONT_END_CYDNEY_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_JIM:
          descriptor.resource_id =
            variant + FRONT_END_JIM_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_PEARL:
          descriptor.resource_id =
            variant + FRONT_END_PEARL_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_JON:
          descriptor.resource_id =
            variant + FRONT_END_JON_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_RHONDA:
          descriptor.resource_id =
            variant + FRONT_END_RHONDA_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_BOSE:
          descriptor.resource_id =
            variant + FRONT_END_BOSE_FINISH_RESOURCE_BASE;
          break;
        case RIDER_TYPE_AXLE:
          descriptor.resource_id =
            variant + FRONT_END_AXLE_FINISH_RESOURCE_BASE;
          break;
        }
    }

  return draw_front_end_cel_descriptor(&descriptor);
}


int
front_end_finalize_race_setup(void)
{
  RiderProfile *profile;
  SignedDivisionResult random_variant;
  int mode;
  int busy;
  int resource_id;

  mode = race_mode_state.outcome_mode;

  if(mode == FRONT_END_OUTCOME_NON_PODIUM)
    {
      random_variant = signed_divide_with_remainder(
        FRONT_END_RANDOM_VARIANT_COUNT, (unsigned int)rand());
      resource_id = (int)random_variant.remainder +
                    FRONT_END_NON_PODIUM_RESOURCE_BASE;
      front_end_preview_state.selected_resource_id = resource_id;
      front_end_cel_group_283[0].resource_id = resource_id;
      apply_front_end_cel_descriptors(front_end_cel_group_283);
    }
  else if(mode == FRONT_END_OUTCOME_BIKE_REPAIR)
    {
      random_variant = signed_divide_with_remainder(
        FRONT_END_RANDOM_VARIANT_COUNT, (unsigned int)rand());
      resource_id = (int)random_variant.remainder +
                    FRONT_END_BIKE_REPAIR_RESOURCE_BASE;
      front_end_preview_state.selected_resource_id = resource_id;
      front_end_cel_group_284[0].resource_id = resource_id;
      apply_front_end_cel_descriptors(front_end_cel_group_284);
    }
  else if(mode == FRONT_END_OUTCOME_FORFEIT)
    {
      random_variant = signed_divide_with_remainder(
        FRONT_END_RANDOM_VARIANT_COUNT, (unsigned int)rand());
      resource_id = (int)random_variant.remainder +
                    FRONT_END_FORFEIT_RESOURCE_BASE;
      front_end_preview_state.selected_resource_id = resource_id;
      front_end_cel_group_285[0].resource_id = resource_id;
      apply_front_end_cel_descriptors(front_end_cel_group_285);
    }
  else if(mode == FRONT_END_OUTCOME_PODIUM ||
          mode == FRONT_END_OUTCOME_ADVANCE_LEVEL)
    {
      profile = &player_profiles[race_mode_state.active_player_index];
      if(mode == FRONT_END_OUTCOME_ADVANCE_LEVEL &&
         profile->level == FRONT_END_MAXIMUM_PROGRESSION_LEVEL)
        {
          apply_front_end_cel_descriptors(front_end_cel_group_282);
        }
      else
        {
          random_variant = signed_divide_with_remainder(
            FRONT_END_RANDOM_VARIANT_COUNT, (unsigned int)rand());
          resource_id = (int)random_variant.remainder +
                        FRONT_END_PODIUM_RESOURCE_BASE;
          front_end_preview_state.selected_resource_id = resource_id;
          front_end_cel_group_281[0].resource_id = resource_id;
          apply_front_end_cel_descriptors(front_end_cel_group_281);
        }

      if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD &&
         race_mode_state.finish_position <=
         FRONT_END_PODIUM_FINISH_MAXIMUM)
        {
          front_end_show_finish_result_descriptor();
        }
    }

  busy = front_end_preview_state.operation_busy;
  if(busy != 0)
    {
      return busy;
    }

  start_next_background_audio_stream();
  return start_front_end_menu_audio();
}
