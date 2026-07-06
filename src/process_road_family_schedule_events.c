#include "family_resource_runtime.h"
#include "rw_semantic_data.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the 0x00020A54 entry of the original function at 0x000209F8. */

void
process_road_family_schedule_events(RoadFamilyScheduleTraversalState *schedule_state,
                                    int                               movement_direction)
{
  RoadFamilyScheduleResource *resource;
  RoadFamilyScheduleEntry *entry;
  int activation_sample;
  int prefetch_sample;
  int entry_index_limit;
  int keep_scanning;
  int family_id;
  int family_group_index;
  int window_span;

  resource = schedule_state->resource;
  if(resource == 0)
    {
      return;
    }

  if(schedule_state->traversal_direction != TRACK_TRAVERSAL_REVERSE)
    {
      activation_sample = schedule_state->sample_index -
                          ROAD_FAMILY_SCHEDULE_FORWARD_WINDOW_BASE +
                          resource->forward_schedule_bias;
      prefetch_sample = activation_sample +
                        (track_runtime_globals.family_schedule_window_span <<
                         ROAD_FAMILY_SCHEDULE_FORWARD_WINDOW_SPAN_SHIFT);
    }
  else
    {
      activation_sample = schedule_state->sample_index +
                          ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_BASE;
      window_span = track_runtime_globals.family_schedule_window_span;
      if(window_span < 0)
        {
          window_span +=
            ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_ROUND_BIAS;
        }
      prefetch_sample = activation_sample -
                        (window_span >>
                         ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_SPAN_SHIFT);
    }

  movement_direction &= TRACK_DIRECTION_BYTE_MASK;
  entry = &resource->entries[schedule_state->entry_index];

  if(movement_direction != TRACK_TRAVERSAL_REVERSE)
    {
      entry_index_limit = resource->entry_count - 1;

      if(schedule_state->traversal_direction ==
         TRACK_TRAVERSAL_REVERSE)
        {
          while(entry->sample_index <= activation_sample)
            {
              if(schedule_state->entry_index >= entry_index_limit)
                {
                  return;
                }
              schedule_state->entry_index++;
              entry++;
            }
          return;
        }

      keep_scanning = 1;
      while(entry->sample_index <= prefetch_sample &&
            keep_scanning != 0)
        {
          keep_scanning = 0;
          if(track_runtime_globals.family_schedule_events_enabled != 0)
            {
              family_id = (int)(entry->packed_family_id &
                                FAMILY_RESOURCE_ID_MASK);
              family_group_index = entry->family_group_index;

              if(entry->boundary_state ==
                 ROAD_FAMILY_SCHEDULE_FORWARD_BOUNDARY)
                {
                  if(enqueue_family_resource_request(
                       family_id, family_group_index,
                       FAMILY_RESOURCE_PREFETCH_MODE) == 0)
                    {
                      return;
                    }
                  entry->boundary_state =
                    ROAD_FAMILY_SCHEDULE_FORWARD_PREFETCH_REQUESTED;
                }

              if(entry->sample_index <= activation_sample)
                {
                  if(entry->boundary_state ==
                     ROAD_FAMILY_SCHEDULE_FORWARD_PREFETCH_REQUESTED)
                    {
                      if(track_runtime_globals
                         .async_family_resource_loads_enabled != 0)
                        {
                          if(enqueue_family_resource_request(
                               family_id, family_group_index,
                               FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE) ==
                             0)
                            {
                              return;
                            }
                        }
                      else
                        {
                          load_family_resource_synchronously(
                            family_id, family_group_index);
                          if(family_resource_globals
                             .resident_family_ids[
                               family_group_index] != family_id)
                            {
                              return;
                            }
                        }
                      entry->boundary_state =
                        ROAD_FAMILY_SCHEDULE_FORWARD_BOUNDARY;
                      keep_scanning = 1;
                    }
                  else if(entry->boundary_state !=
                          ROAD_FAMILY_SCHEDULE_FORWARD_BOUNDARY)
                    {
                      keep_scanning = 1;
                    }
                }
            }
          else
            {
              keep_scanning = 1;
            }

          if(keep_scanning != 0)
            {
              if(schedule_state->entry_index < entry_index_limit)
                {
                  schedule_state->entry_index++;
                  entry++;
                }
              else
                {
                  keep_scanning = 0;
                }
            }
        }
      return;
    }

  if(schedule_state->traversal_direction != TRACK_TRAVERSAL_REVERSE)
    {
      while(entry->sample_index >= activation_sample)
        {
          if(schedule_state->entry_index <= 0)
            {
              return;
            }
          schedule_state->entry_index--;
          entry--;
        }
      return;
    }

  keep_scanning = 1;
  while(entry->sample_index >= prefetch_sample &&
        keep_scanning != 0)
    {
      keep_scanning = 0;
      if(track_runtime_globals.family_schedule_events_enabled != 0)
        {
          family_id = (int)(entry->packed_family_id &
                            FAMILY_RESOURCE_ID_MASK);
          family_group_index = entry->family_group_index;

          if(entry->boundary_state ==
             ROAD_FAMILY_SCHEDULE_REVERSE_BOUNDARY)
            {
              if(enqueue_family_resource_request(
                   family_id, family_group_index,
                   FAMILY_RESOURCE_PREFETCH_MODE) == 0)
                {
                  return;
                }
              entry->boundary_state =
                ROAD_FAMILY_SCHEDULE_REVERSE_PREFETCH_REQUESTED;
            }

          if(entry->sample_index < activation_sample)
            {
              return;
            }

          if(entry->boundary_state ==
             ROAD_FAMILY_SCHEDULE_REVERSE_PREFETCH_REQUESTED)
            {
              if(enqueue_family_resource_request(
                   family_id, family_group_index,
                   FAMILY_RESOURCE_LOAD_AND_FIXUP_MODE) == 0)
                {
                  return;
                }
              entry->boundary_state =
                ROAD_FAMILY_SCHEDULE_REVERSE_BOUNDARY;
              keep_scanning = 1;
            }
          else if(entry->boundary_state !=
                  ROAD_FAMILY_SCHEDULE_REVERSE_BOUNDARY)
            {
              keep_scanning = 1;
            }
        }
      else
        {
          keep_scanning = 1;
        }

      if(keep_scanning != 0)
        {
          if(schedule_state->entry_index > 0)
            {
              schedule_state->entry_index--;
              entry--;
            }
          else
            {
              keep_scanning = 0;
            }
        }
    }
}
