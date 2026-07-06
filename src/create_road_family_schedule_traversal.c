#include "memory_manager_runtime.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x000209F8. */

RoadFamilyScheduleTraversalState *
create_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                      RoadFamilyScheduleResource       *resource,
                                      int                               traversal_direction)
{
  traversal_direction &= TRACK_DIRECTION_BYTE_MASK;
  if(schedule_state == 0)
    {
      schedule_state =
        (RoadFamilyScheduleTraversalState *)MEM_NewPointer(
          sizeof(*schedule_state), MEMORY_REQUEST_ANY,
          MEM_POINTER_OPTIONS_DEFAULT);
    }

  if(schedule_state != 0)
    {
      initialize_road_family_schedule_traversal(
        schedule_state, resource, 0);
      schedule_state->traversal_direction =
        (unsigned char)traversal_direction;
    }

  return schedule_state;
}
