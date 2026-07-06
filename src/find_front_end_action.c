/* Reconstructed from the original function at 0x00008DC4. */

#include "stdlib.h"

#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

enum FrontEndCelPreloadConstant
{
  FRONT_END_CEL_MINIMUM_DESCRIPTOR_COUNT = 3,
  FRONT_END_CEL_PRELOAD_CAPACITY = 10,
  FRONT_END_CEL_MINIMUM_ADJACENT_SPAN = 2
};


FrontEndAction *
find_front_end_action(int action_id)
{
  FrontEndAction *action;

  action = front_end_actions;
  while(action->id != -1)
    {
      if(action->id == action_id)
        {
          return action;
        }
      action++;
    }
  return 0;
}


static
int
compare_resource_indices(const void *left_value,
                         const void *right_value)
{
  int left;
  int right;

  left = *(const int *)left_value;
  right = *(const int *)right_value;
  if(left < right)
    {
      return -1;
    }
  if(left != right)
    {
      return 1;
    }
  return 0;
}


void
preload_adjacent_front_end_cels(const FrontEndCelDescriptor *descriptors)
{
  int resource_indices[FRONT_END_CEL_PRELOAD_CAPACITY];
  int count;
  int resource_index;
  int next_resource_index;
  int index;
  int first_adjacent_index;
  int last_adjacent_index;
  int span;

  count = 0;
  while(count < FRONT_END_CEL_PRELOAD_CAPACITY &&
        descriptors[count].resource_id != -1)
    {
      resource_indices[count] = descriptors[count].resource_id + 1;
      count++;
    }

  if(count < FRONT_END_CEL_MINIMUM_DESCRIPTOR_COUNT)
    {
      return;
    }

  qsort(resource_indices, count, sizeof(resource_indices[0]),
        compare_resource_indices);

  first_adjacent_index = 0;
  last_adjacent_index = 0;
  index = 0;
  while(index < count - 1)
    {
      resource_index = resource_indices[index];
      next_resource_index = resource_indices[index + 1];
      if(resource_index + 1 == next_resource_index)
        {
          if(first_adjacent_index == 0)
            {
              first_adjacent_index = resource_index;
            }
          else
            {
              last_adjacent_index = next_resource_index;
            }
        }
      else if(first_adjacent_index != 0)
        {
          last_adjacent_index = resource_index;
          break;
        }
      index++;
    }

  span = last_adjacent_index - first_adjacent_index;
  if(span <= 0)
    {
      span = -span;
    }
  if(span >= FRONT_END_CEL_MINIMUM_ADJACENT_SPAN &&
     last_adjacent_index != 0)
    {
      RSRC_LoadAdjacentResources(
        CANS_RESOURCE_TYPE_CEL,
        (unsigned int)first_adjacent_index,
        (unsigned int)last_adjacent_index);
    }
}
