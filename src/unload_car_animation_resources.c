#include "string.h"

#include "rw_semantic_data.h"

int
RSRC_DisposeFileResources(int resource_file_item);

void
unload_car_animation_resources(void)
{
  CarAnimationResourcePair *resources;
  CelAnimationBinding **bindings;
  int animation_index;
  int binding_index;
  int resource_file_item;

  resource_file_item = car_animation_load_state.resource_file_item;
  for(animation_index = 0;
      animation_index < CAR_ANIMATION_COUNT;
      animation_index++)
    {
      resources = &car_animation_resources[animation_index];
      resources->animation_resource = 0;
      resources->cans_resource = 0;
      car_animation_output_info[animation_index] = 0;
      bindings = car_animation_bindings_by_car[animation_index];
      for(binding_index = 0;
          binding_index < CAR_ANIMATION_VARIANT_COUNT;
          binding_index++)
        {
          memset(bindings[binding_index], 0,
                 sizeof(*bindings[binding_index]));
          bindings[binding_index]->entry_index = -1;
        }
    }

  car_animation_load_state.animation_count = 0;
  car_animation_load_state.resource_file_item = 0;
  if(resource_file_item != 0)
    {
      RSRC_DisposeFileResources(resource_file_item);
    }
}
