#include "cans_animation_binding.h"
#include "division_helpers.h"

enum
{
  CANS_MATCHING_GROUP_LIMIT = 3,
  CANS_GROUP_PREVIOUS = 0,
  CANS_GROUP_SELECTED = 1,
  CANS_GROUP_NEXT = 2,
  CANS_ENTRY_AXIS_FLAGS = 0x0006,
  CANS_ENTRY_HORIZONTAL_AXIS = 0x0002,
  CANS_ENTRY_VERTICAL_AXIS = 0x0004,
  CANS_ENTRY_VERTICAL_WEIGHT_FIRST = 0x0020,
  CANS_ENTRY_VERTICAL_WEIGHT_MAXIMUM = 0x0400,
  CANS_ENTRY_HORIZONTAL_WEIGHT_FIRST = 0x0800,
  CANS_ENTRY_HORIZONTAL_WEIGHT_MAXIMUM = 0x8000,
  CANS_ENTRY_VARIANT_GRID_FLAGS = 0xF800,
  CANS_HORIZONTAL_WEIGHT_COUNT = 4,
  CANS_VERTICAL_WEIGHT_COUNT = 5,
  CANS_WEIGHT_FRACTION_SHIFT = 16,
  CANS_EXTENT_SCALE_SHIFT = 1,
  CANS_HORIZONTAL_MAXIMUM_WEIGHT = 0x00200000,
  CANS_VERTICAL_MAXIMUM_WEIGHT = 0x00400000,
  CANS_DURATION_TICK_DIVISOR = 20
};

static const int cans_direction_weight_by_level[CANS_VERTICAL_WEIGHT_COUNT] =
{
  0x0002AAAA,
  0x00055555,
  0x000AAAAA,
  0x00155555,
  0x002AAAAA
};

static
void
swap_animation_group_ranges(int *left_start,
                            int *left_end,
                            int *right_start,
                            int *right_end)
{
  int saved_index;

  saved_index = *left_start;
  *left_start = *right_start;
  *right_start = saved_index;

  saved_index = *left_end;
  *left_end = *right_end;
  *right_end = saved_index;
}


int
resolve_cans_animation_binding(const AnimationResourcePair *resources,
                               const AnimationBindingSpec  *spec,
                               CelAnimationBinding         *binding)
{
  int group_start[CANS_MATCHING_GROUP_LIMIT];
  int group_end[CANS_MATCHING_GROUP_LIMIT];
  int resource_key;
  const CansAnimationNameRecord *names;
  int search_index;
  signed char group_slot;
  int flags;
  const CansAnimationGroup *group;
  int direction_weight;
  int direction_flag;
  int level;
  SignedDivisionResult division;

  resource_key = spec->resource_key;
  if(resource_key < 0)
    {
      resource_key = -resource_key;
    }

  names = get_cans_animation_name_table(resources, 0);
  search_index = 0;
  group_slot = 0;
  do
    {
      search_index =
        find_cans_animation_name(resources, resource_key, search_index);
      group_start[group_slot] = search_index;
      if(search_index < 0)
        {
          break;
        }

      flags = get_cans_animation_entry_flags(
        resources, (unsigned int)search_index, 0);
      group_end[group_slot] = group_start[group_slot];
      do
        {
          group_end[group_slot]++;
          if(names[group_end[group_slot]].resource_key != resource_key)
            {
              break;
            }
        } while((get_cans_animation_entry_flags(
                   resources, (unsigned int)group_end[group_slot], 0) &
                 CANS_ENTRY_AXIS_FLAGS) ==
                (flags & CANS_ENTRY_AXIS_FLAGS));

      search_index = group_end[group_slot];
      group_slot = (signed char)(group_slot + 1);
    } while(group_slot < CANS_MATCHING_GROUP_LIMIT);

  if(group_start[CANS_GROUP_PREVIOUS] < 0)
    {
      return 0;
    }

  if(group_start[CANS_GROUP_SELECTED] < 0)
    {
      group_start[CANS_GROUP_SELECTED] = group_start[CANS_GROUP_PREVIOUS];
      group_end[CANS_GROUP_SELECTED] = group_end[CANS_GROUP_PREVIOUS];
      group_start[CANS_GROUP_NEXT] = -1;
      group_end[CANS_GROUP_NEXT] = -1;
      group_start[CANS_GROUP_PREVIOUS] = -1;
      group_end[CANS_GROUP_PREVIOUS] = -1;
    }
  else if(group_start[CANS_GROUP_NEXT] < 0)
    {
      group_end[CANS_GROUP_NEXT] = -1;
      flags = get_cans_animation_entry_flags(
        resources, (unsigned int)group_start[CANS_GROUP_SELECTED], 0);
      if((flags & CANS_ENTRY_AXIS_FLAGS) != 0)
        {
          flags = get_cans_animation_entry_flags(
            resources, (unsigned int)group_start[CANS_GROUP_SELECTED], 0);
          if((flags & CANS_ENTRY_HORIZONTAL_AXIS) == 0)
            {
              group_start[CANS_GROUP_NEXT] =
                group_start[CANS_GROUP_SELECTED];
              group_end[CANS_GROUP_NEXT] =
                group_end[CANS_GROUP_SELECTED];
              group_start[CANS_GROUP_SELECTED] =
                group_start[CANS_GROUP_PREVIOUS];
              group_end[CANS_GROUP_SELECTED] =
                group_end[CANS_GROUP_PREVIOUS];
              group_start[CANS_GROUP_PREVIOUS] = -1;
              group_end[CANS_GROUP_PREVIOUS] = -1;
            }
          else
            {
              swap_animation_group_ranges(
                &group_start[CANS_GROUP_PREVIOUS],
                &group_end[CANS_GROUP_PREVIOUS],
                &group_start[CANS_GROUP_SELECTED],
                &group_end[CANS_GROUP_SELECTED]);
            }
        }
    }
  else
    {
      flags = get_cans_animation_entry_flags(
        resources, (unsigned int)group_start[CANS_GROUP_PREVIOUS], 0);
      if((flags & CANS_ENTRY_HORIZONTAL_AXIS) == 0)
        {
          flags = get_cans_animation_entry_flags(
            resources, (unsigned int)group_start[CANS_GROUP_SELECTED], 0);
          if((flags & CANS_ENTRY_HORIZONTAL_AXIS) == 0)
            {
              swap_animation_group_ranges(
                &group_start[CANS_GROUP_PREVIOUS],
                &group_end[CANS_GROUP_PREVIOUS],
                &group_start[CANS_GROUP_NEXT],
                &group_end[CANS_GROUP_NEXT]);
            }
          else
            {
              swap_animation_group_ranges(
                &group_start[CANS_GROUP_PREVIOUS],
                &group_end[CANS_GROUP_PREVIOUS],
                &group_start[CANS_GROUP_SELECTED],
                &group_end[CANS_GROUP_SELECTED]);
            }
        }

      flags = get_cans_animation_entry_flags(
        resources, (unsigned int)group_start[CANS_GROUP_SELECTED], 0);
      if((flags & CANS_ENTRY_VERTICAL_AXIS) != 0)
        {
          swap_animation_group_ranges(
            &group_start[CANS_GROUP_SELECTED],
            &group_end[CANS_GROUP_SELECTED],
            &group_start[CANS_GROUP_NEXT],
            &group_end[CANS_GROUP_NEXT]);
        }
    }

  group = find_cans_animation_group(
    resources, group_start[CANS_GROUP_SELECTED]);
  flags = get_cans_animation_entry_flags(
    resources, (unsigned int)group_start[CANS_GROUP_SELECTED], 0);

  binding->entry_index = group_start[CANS_GROUP_SELECTED];
  binding->channel = spec->channel;

  if((flags & CANS_ENTRY_VARIANT_GRID_FLAGS) == 0)
    {
      binding->uses_variant_grid = 0;
      binding->horizontal_variant_count = 0;
      binding->vertical_variant_count = (unsigned char)group->variant_count;
    }
  else
    {
      binding->uses_variant_grid = 1;
      binding->horizontal_variant_count = (unsigned char)group->variant_count;
      binding->vertical_variant_count =
        (unsigned char)(group_end[CANS_GROUP_SELECTED] -
                        group_start[CANS_GROUP_SELECTED]);
    }

  direction_weight = 0;
  direction_flag = CANS_ENTRY_HORIZONTAL_WEIGHT_FIRST;
  for(level = 0; level < CANS_HORIZONTAL_WEIGHT_COUNT; level++)
    {
      if((flags & direction_flag) != 0)
        {
          direction_weight += cans_direction_weight_by_level[level];
        }
      direction_flag <<= 1;
    }
  if((flags & CANS_ENTRY_HORIZONTAL_WEIGHT_MAXIMUM) != 0)
    {
      direction_weight += CANS_HORIZONTAL_MAXIMUM_WEIGHT;
    }
  binding->horizontal_extent = (unsigned char)
                               ((direction_weight >> CANS_WEIGHT_FRACTION_SHIFT) <<
                                CANS_EXTENT_SCALE_SHIFT);

  direction_weight = 0;
  direction_flag = CANS_ENTRY_VERTICAL_WEIGHT_FIRST;
  for(level = 0; level < CANS_VERTICAL_WEIGHT_COUNT; level++)
    {
      if((flags & direction_flag) != 0)
        {
          direction_weight += cans_direction_weight_by_level[level];
        }
      direction_flag <<= 1;
    }
  if((flags & CANS_ENTRY_VERTICAL_WEIGHT_MAXIMUM) != 0)
    {
      direction_weight += CANS_VERTICAL_MAXIMUM_WEIGHT;
    }
  binding->vertical_extent = (unsigned char)
                             ((direction_weight >> CANS_WEIGHT_FRACTION_SHIFT) <<
                              CANS_EXTENT_SCALE_SHIFT);

  if(group_start[CANS_GROUP_PREVIOUS] < 0)
    {
      binding->previous_entry_offset = 0;
    }
  else
    {
      binding->previous_entry_offset =
        (signed char)(group_start[CANS_GROUP_PREVIOUS] -
                      group_start[CANS_GROUP_SELECTED]);
    }

  if(group_start[CANS_GROUP_NEXT] < 0)
    {
      binding->next_entry_offset = 0;
    }
  else
    {
      binding->next_entry_offset =
        (signed char)(group_start[CANS_GROUP_NEXT] -
                      group_start[CANS_GROUP_SELECTED]);
    }

  division = signed_divide_with_remainder(
    CANS_DURATION_TICK_DIVISOR, (unsigned int)group->duration);
  binding->duration_ticks = (signed char)division.quotient;
  binding->transition_command = spec->transition_command;
  binding->frame_flags = get_cans_animation_flag_table(
    resources, (unsigned int)group_start[CANS_GROUP_SELECTED]);

  return 1;
}
