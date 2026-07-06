#include "string.h"

#include "cans_animation_binding.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

enum CarAnimationResourceConstant
{
  CAR_RESOURCE_PATH_CAPACITY = 80,
  CAR_RESOURCE_FILE_OPTIONS = 0x1000,
  CAR_RESOURCE_LOAD_OPTIONS = 8,
  CAR_RESOURCE_INDEX_BASE = 1,
  CAR_ANIMATION_SECONDARY_BINDING = 2
};

#define RESOURCE_TAG_ANIMATION 0x414E494D
#define RESOURCE_TAG_OFFSETS   0x4F465353
#define RESOURCE_TAG_CANS      0x43414E53

int *
RSRC_LoadResource(int type,
                  int index,
                  int flags);

void
load_car_animation_resources(void)
{
  char path[CAR_RESOURCE_PATH_CAPACITY];
  CansAnimationFrameOutput frame_output;
  signed char animation_index;
  signed char cans_index;
  int *resource;
  int *animation;
  CarAnimationResourcePair *resources;
  CelAnimationBinding **bindings;
  char *variant_name;
  int player_index;
  int variant_index;
  int resource_file_item;

  player_index = race_mode_state.active_player_index;
  variant_index = player_profiles[player_index].current_variant;
  variant_name = course_variant_names[variant_index];

  strcpy(path, (char *)(int)&resource_root_path);
  strcat(path, variant_name);
  strcat(path, "/");
  strcat(path, variant_name);
  strcat(path, (char *)(int)&car_resource_suffix);

  resource_file_item =
    (int)RSRC_OpenFile(path, CAR_RESOURCE_FILE_OPTIONS);
  car_animation_load_state.resource_file_item = resource_file_item;
  if(resource_file_item == 0)
    {
      return;
    }

  animation_index = 0;
  cans_index = 0;
  do
    {
      resource = RSRC_LoadResource(
        RESOURCE_TAG_ANIMATION,
        animation_index + CAR_RESOURCE_INDEX_BASE,
        CAR_RESOURCE_LOAD_OPTIONS);
      if(resource == 0)
        {
          car_animation_load_state.animation_count =
            (unsigned char)animation_index;
          return;
        }

      animation = (int *)resource[0];
      resources = &car_animation_resources[animation_index];
      resources->animation_resource = animation;

      if(animation[0] == RESOURCE_TAG_OFFSETS)
        {
          CANS_RelocateAnimationOffsets(
            (CansOffsetAnimation *)animation);
        }

      resource = RSRC_LoadResource(
        RESOURCE_TAG_CANS,
        cans_index + CAR_RESOURCE_INDEX_BASE,
        CAR_RESOURCE_LOAD_OPTIONS);
      if(resource == 0)
        {
          if(animation_index == 0)
            {
              car_animation_load_state.animation_count = 0;
              return;
            }
          resources->cans_resource = resources[-1].cans_resource;
        }
      else
        {
          resources->cans_resource = (void *)resource[0];
          cans_index = (signed char)(cans_index + 1);
        }

      bindings = car_animation_bindings_by_car[animation_index];
      if(resolve_cans_animation_bindings(
           resources, &car_animation_selectors[0],
           bindings[0], 1) == 0)
        {
          bindings[0]->entry_index = -1;
        }
      if(resolve_cans_animation_bindings(
           resources, &car_animation_selectors[1],
           bindings[1], 1) == 0)
        {
          bindings[1]->entry_index = -1;
        }
      if(resolve_cans_animation_bindings(
           resources,
           &car_animation_selectors[
             CAR_ANIMATION_SECONDARY_BINDING],
           bindings[CAR_ANIMATION_SECONDARY_BINDING], 1) == 0)
        {
          bindings[CAR_ANIMATION_SECONDARY_BINDING]
          ->entry_index = -1;
        }

      decode_cans_animation_frame(resources, 0, 0, &frame_output);
      car_animation_output_info[animation_index] =
        frame_output.animation_flags;
      car_animation_load_state.animation_count =
        (unsigned char)(animation_index + 1);

      animation_index = (signed char)(animation_index + 1);
    } while(animation_index < CAR_ANIMATION_COUNT);
}


#undef RESOURCE_TAG_ANIMATION
#undef RESOURCE_TAG_OFFSETS
#undef RESOURCE_TAG_CANS
