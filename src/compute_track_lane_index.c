#include "division_helpers.h"
#include "rider_visual_runtime.h"
#include "track_segment_definition.h"

enum TrackLaneIndexValue
{
  TRACK_LANE_PROBE_ODD_MASK = 1,
  TRACK_LANE_PROBE_ZERO_RESULT = 3,
  TRACK_LANE_POSITION_FRACTION_BITS = 8,
  TRACK_LANE_SPACING_FIXED = 0xFA00,
  TRACK_LANE_LINK_INDEX = 2,
  TRACK_LANE_WALK_LIMIT = 0x7F,
  TRACK_LANE_FORWARD_BOUNDARY_FLAG = 0x10,
  TRACK_LANE_REVERSE_BOUNDARY_FLAG = 0x08
};

int
compute_track_lane_index(const RiderVisualObject *rider,
                         int                      probe_mode)
{
  int negative_adjustment;
  int lateral_delta;

  negative_adjustment = 0;
  if((probe_mode & TRACK_LANE_PROBE_ODD_MASK) != 0)
    {
      if(probe_mode != TRACK_LANE_PROBE_ZERO_RESULT)
        {
          negative_adjustment = -1;
        }
      return (signed char)negative_adjustment;
    }

  lateral_delta = rider->position.lateral_position -
                  (rider->track_segment->center_position <<
                   TRACK_LANE_POSITION_FRACTION_BITS);
  if(lateral_delta < 0)
    {
      negative_adjustment = 1;
    }

  return (signed char)(divide_lane_offset_by_spacing(lateral_delta) -
                       negative_adjustment);
}


void
build_road_section_entry_from_lane_span(RoadSegmentLaneRuntime *lane,
                                        RoadSectionEntry       *entry)
{
  RoadSegmentLaneRuntime *forward_boundary;
  RoadSegmentRuntime *segment;
  int remaining;

  remaining = TRACK_LANE_WALK_LIMIT;
  while((lane->surface_flags & TRACK_LANE_FORWARD_BOUNDARY_FLAG) == 0)
    {
      if(remaining == 0)
        {
          break;
        }
      remaining--;
      lane = lane->previous_links[TRACK_LANE_LINK_INDEX];
    }

  forward_boundary = lane;
  remaining = TRACK_LANE_WALK_LIMIT;
  while((lane->surface_flags & TRACK_LANE_REVERSE_BOUNDARY_FLAG) == 0)
    {
      if(remaining == 0)
        {
          break;
        }
      remaining--;
      lane = lane->next_links[TRACK_LANE_LINK_INDEX];
    }

  segment = (RoadSegmentRuntime *)lane->resource_handle;
  entry->start_sample = segment->track_position;
  segment = (RoadSegmentRuntime *)forward_boundary->resource_handle;
  entry->end_sample = segment->track_position;
  entry->kind = lane->collision_kind;
  entry->selector = (signed char)lane->surface_selector;
  entry->collision_inset = lane->collision_inset;
  entry->edge_collision_flags = lane->edge_collision_flags;
}
