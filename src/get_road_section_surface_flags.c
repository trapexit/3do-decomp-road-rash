#include "bss_late_data.h"

/* Reconstructed from the original function at 0x00021184. */

enum RoadSectionSurfaceConstants
{
  ROAD_SECTION_ACTIVE_KIND_INTERPOLATED = 1,
  ROAD_SECTION_ACTIVE_KIND_GEOMETRY_OVERRIDE = 4,
  ROAD_SECTION_ACTIVE_KIND_NO_SURFACE_FLAGS = 5,
  ROAD_SECTION_ACTIVE_KIND_LEVEL_GATE = 7,
  ROAD_SECTION_ENTRY_FLAG = 0x08,
  ROAD_SECTION_PRE_TERMINAL_FLAG = 0x10,
  ROAD_SECTION_INTERIOR_FLAG_BASE = 0x20,
  ROAD_SECTION_GEOMETRY_OVERRIDE_FLAG = 0x40,
  ROAD_SECTION_BALANCED_INTERIOR_VALUE = 2
};

#define ROAD_SECTION_TERMINAL_FLAG ((int)0x80000000)

int
get_road_section_surface_flags(const RoadSectionTraversalState *traversal)
{
  const RoadSectionEntry *entry;
  int sample_position;
  int start_sample;
  int end_sample;
  int active_kind;
  int to_end;
  int from_start;
  int result;

  if(traversal->resource == 0)
    {
      return 0;
    }

  sample_position =
    traversal->position >> TRACK_POSITION_FRACTION_BITS;
  entry = traversal->active_entry;
  start_sample = entry->start_sample;
  if(sample_position < start_sample)
    {
      return 0;
    }

  end_sample = entry->end_sample;
  if(sample_position > end_sample)
    {
      return 0;
    }

  active_kind = traversal->active_kind;
  if(active_kind == ROAD_SECTION_ACTIVE_KIND_GEOMETRY_OVERRIDE)
    {
      return ROAD_SECTION_GEOMETRY_OVERRIDE_FLAG;
    }
  if(active_kind == ROAD_SECTION_ACTIVE_KIND_NO_SURFACE_FLAGS)
    {
      return 0;
    }
  if(active_kind == ROAD_SECTION_ACTIVE_KIND_LEVEL_GATE)
    {
      return 0;
    }
  if(sample_position == end_sample)
    {
      return ROAD_SECTION_TERMINAL_FLAG;
    }
  if(sample_position == start_sample)
    {
      return ROAD_SECTION_ENTRY_FLAG;
    }
  if(end_sample - 1 == sample_position)
    {
      return ROAD_SECTION_PRE_TERMINAL_FLAG;
    }

  if(active_kind == ROAD_SECTION_ACTIVE_KIND_INTERPOLATED)
    {
      to_end = end_sample - sample_position;
      from_start = sample_position - start_sample;
      result = to_end < from_start + 1;
      if((to_end - from_start) == 1 || to_end == from_start)
        {
          result = ROAD_SECTION_BALANCED_INTERIOR_VALUE;
        }
    }
  else
    {
      result = 1;
    }

  return result + ROAD_SECTION_INTERIOR_FLAG_BASE;
}


#undef ROAD_SECTION_TERMINAL_FLAG
