#include "bss_late_data.h"
#include "division_helpers.h"
#include "road_geometry.h"

/* Reconstructed from the original function at 0x00013D1C. */

enum RoadCrossSectionValue
{
  ROAD_CROSS_SECTION_UNAVAILABLE = 0x3E7,
  ROAD_CROSS_SECTION_INTERPOLATION_MASK = 0xFF,
  ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR = 2,
  ROAD_SURFACE_FLAG_FLAT_OUTSIDE = 0x20,
  ROAD_GEOMETRY_MODE_EXCLUSION = 5,
  ROAD_PROFILE_LEFT_INNER = 0,
  ROAD_PROFILE_RIGHT_INNER = 1,
  ROAD_PROFILE_LEFT_MIDDLE = 2,
  ROAD_PROFILE_RIGHT_MIDDLE = 3,
  ROAD_PROFILE_LEFT_OUTER = 4,
  ROAD_PROFILE_RIGHT_OUTER = 5
};

static
int
interpolate_cross_section_value(int current,
                                int next,
                                int fraction)
{
  int delta;

  delta = fraction * (next - current);
  if(delta < 0)
    {
      delta += ROAD_CROSS_SECTION_INTERPOLATION_MASK;
    }
  return current + (delta >> TRACK_POSITION_FRACTION_BITS);
}


static
int
interpolate_cross_section_segment(int position,
                                  int low_position,
                                  int high_position,
                                  int low_height,
                                  int high_height)
{
  SignedDivisionResult division;

  division = signed_divide_with_remainder(
    (unsigned int)(high_position - low_position),
    (unsigned int)((position - low_position) *
                   (high_height - low_height)));
  return low_height + division.quotient;
}


static
int
sample_profile_right_height(const RoadSegmentLaneRuntime *lane,
                            const RoadSegmentLaneRuntime *next_lane,
                            int                           position,
                            int                           fraction)
{
  int low_position;
  int high_position;
  int low_height;
  int high_height;
  int profile_index;

  high_position = interpolate_cross_section_value(
    lane->shoulder_right, next_lane->shoulder_right, fraction);
  if(position < high_position)
    {
      return lane->edge_depth_offsets[ROAD_SIDE_RIGHT];
    }

  low_position = high_position;
  high_position = interpolate_cross_section_value(
    lane->surface_profile[ROAD_PROFILE_RIGHT_INNER].x,
    next_lane->surface_profile[ROAD_PROFILE_RIGHT_INNER].x,
    fraction);
  if(position <= high_position)
    {
      low_height = interpolate_cross_section_value(
        lane->edge_depth_offsets[ROAD_SIDE_RIGHT],
        next_lane->edge_depth_offsets[ROAD_SIDE_RIGHT], fraction);
      high_height = interpolate_cross_section_value(
        lane->surface_profile[ROAD_PROFILE_RIGHT_INNER].y,
        next_lane->surface_profile[ROAD_PROFILE_RIGHT_INNER].y,
        fraction);
      return interpolate_cross_section_segment(
        position, low_position, high_position,
        low_height, high_height);
    }

  low_position = high_position;
  high_position = interpolate_cross_section_value(
    lane->surface_profile[ROAD_PROFILE_RIGHT_MIDDLE].x,
    next_lane->surface_profile[ROAD_PROFILE_RIGHT_MIDDLE].x,
    fraction);
  if(position <= high_position)
    {
      profile_index = ROAD_PROFILE_RIGHT_INNER;
    }
  else
    {
      low_position = high_position;
      high_position = interpolate_cross_section_value(
        lane->surface_profile[ROAD_PROFILE_RIGHT_OUTER].x,
        next_lane->surface_profile[ROAD_PROFILE_RIGHT_OUTER].x,
        fraction);
      if(position > high_position)
        {
          return interpolate_cross_section_value(
            lane->surface_profile[ROAD_PROFILE_RIGHT_OUTER].y,
            next_lane->surface_profile[ROAD_PROFILE_RIGHT_OUTER].y,
            fraction);
        }
      profile_index = ROAD_PROFILE_RIGHT_MIDDLE;
    }

  low_height = interpolate_cross_section_value(
    lane->surface_profile[profile_index].y,
    next_lane->surface_profile[profile_index].y, fraction);
  high_height = interpolate_cross_section_value(
    lane->surface_profile[profile_index +
                          ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR].y,
    next_lane->surface_profile[profile_index +
                               ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR].y,
    fraction);
  return interpolate_cross_section_segment(
    position, low_position, high_position,
    low_height, high_height);
}


static
int
sample_profile_left_height(const RoadSegmentLaneRuntime *lane,
                           const RoadSegmentLaneRuntime *next_lane,
                           int                           position,
                           int                           fraction)
{
  int segment_start;
  int segment_end;
  int start_height;
  int end_height;
  int profile_index;

  segment_start = interpolate_cross_section_value(
    lane->road_left, next_lane->road_left, fraction);
  if(position >= segment_start)
    {
      return 0;
    }

  segment_start = interpolate_cross_section_value(
    lane->shoulder_left, next_lane->shoulder_left, fraction);
  if(position > segment_start)
    {
      return lane->edge_depth_offsets[ROAD_SIDE_LEFT];
    }

  segment_end = interpolate_cross_section_value(
    lane->surface_profile[ROAD_PROFILE_LEFT_INNER].x,
    next_lane->surface_profile[ROAD_PROFILE_LEFT_INNER].x,
    fraction);
  if(position >= segment_end)
    {
      start_height = interpolate_cross_section_value(
        lane->edge_depth_offsets[ROAD_SIDE_LEFT],
        next_lane->edge_depth_offsets[ROAD_SIDE_LEFT], fraction);
      end_height = interpolate_cross_section_value(
        lane->surface_profile[ROAD_PROFILE_LEFT_INNER].y,
        next_lane->surface_profile[ROAD_PROFILE_LEFT_INNER].y,
        fraction);
      return interpolate_cross_section_segment(
        position, segment_start, segment_end,
        start_height, end_height);
    }

  segment_start = segment_end;
  segment_end = interpolate_cross_section_value(
    lane->surface_profile[ROAD_PROFILE_LEFT_MIDDLE].x,
    next_lane->surface_profile[ROAD_PROFILE_LEFT_MIDDLE].x,
    fraction);
  if(position >= segment_end)
    {
      profile_index = ROAD_PROFILE_LEFT_INNER;
    }
  else
    {
      segment_start = segment_end;
      segment_end = interpolate_cross_section_value(
        lane->surface_profile[ROAD_PROFILE_LEFT_OUTER].x,
        next_lane->surface_profile[ROAD_PROFILE_LEFT_OUTER].x,
        fraction);
      if(position < segment_end)
        {
          return interpolate_cross_section_value(
            lane->surface_profile[ROAD_PROFILE_LEFT_OUTER].y,
            next_lane->surface_profile[ROAD_PROFILE_LEFT_OUTER].y,
            fraction);
        }
      profile_index = ROAD_PROFILE_LEFT_MIDDLE;
    }

  start_height = interpolate_cross_section_value(
    lane->surface_profile[profile_index].y,
    next_lane->surface_profile[profile_index].y, fraction);
  end_height = interpolate_cross_section_value(
    lane->surface_profile[profile_index +
                          ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR].y,
    next_lane->surface_profile[profile_index +
                               ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR].y,
    fraction);
  return interpolate_cross_section_segment(
    position, segment_start, segment_end,
    start_height, end_height);
}


static
int
sample_edge_shape_height(const RoadSegmentLaneRuntime *lane,
                         int                           position)
{
  const RoadEdgeShape *shape;
  SignedDivisionResult division;
  int threshold;
  int span;
  int slope;
  int absolute_slope;

  shape = &lane->edge_shapes[ROAD_SIDE_RIGHT];
  threshold = shape->inner_offset;
  if(position > threshold)
    {
      position -= threshold;
      span = shape->outer_offset - threshold;
      slope = shape->height;
      absolute_slope = slope;
      if(absolute_slope < 0)
        {
          absolute_slope = -absolute_slope;
        }
      if(slope == 0)
        {
          return 0;
        }
      if(span > absolute_slope *
         ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR &&
         shape->outer_offset > position)
        {
          division = signed_divide_with_remainder(
            (unsigned int)span,
            (unsigned int)(slope * position));
          return division.quotient;
        }
      return slope;
    }

  shape = &lane->edge_shapes[ROAD_SIDE_LEFT];
  threshold = shape->inner_offset;
  if(position >= threshold)
    {
      return 0;
    }

  position = threshold - position;
  span = threshold - shape->outer_offset;
  slope = shape->height;
  absolute_slope = slope;
  if(absolute_slope < 0)
    {
      absolute_slope = -absolute_slope;
    }
  if(slope == 0)
    {
      return 0;
    }
  if(span > absolute_slope *
     ROAD_CROSS_SECTION_EDGE_SLOPE_FACTOR &&
     position > shape->outer_offset)
    {
      division = signed_divide_with_remainder(
        (unsigned int)span,
        (unsigned int)(slope *
                       (lane->road_left - position)));
      return division.quotient;
    }
  return slope;
}


int
sample_road_cross_section_height(const RoadSegmentLaneRuntime *lane_runtime,
                                 int                           lateral_position,
                                 unsigned int                  interpolation_fraction)
{
  const RoadSegmentLaneRuntime *next_lane;
  int position;
  int fraction;
  unsigned char flags;

  position = lateral_position >> TRACK_POSITION_FRACTION_BITS;
  if(lane_runtime->road_left <= position &&
     position <= lane_runtime->road_right)
    {
      return 0;
    }

  switch(lane_runtime->geometry_mode)
    {
    case ROAD_GEOMETRY_MODE_TEXTURED:
      flags = lane_runtime->surface_flags;
      if((flags & ROAD_SURFACE_FLAG_FLAT_OUTSIDE) != 0)
        {
          return 0;
        }
      if(position <= lane_runtime->road_right)
        {
          if(position <= lane_runtime->shoulder_left &&
             (flags & ROAD_SURFACE_FLAG_MARGIN_MASK) == 0 &&
             lane_runtime->surface_resource_ids[ROAD_SIDE_LEFT] !=
             ROAD_SURFACE_RESOURCE_MISSING)
            {
              return ROAD_CROSS_SECTION_UNAVAILABLE;
            }
          return lane_runtime->edge_depth_offsets[ROAD_SIDE_LEFT];
        }
      if(lane_runtime->shoulder_right <= position &&
         (flags & ROAD_SURFACE_FLAG_MARGIN_MASK) == 0 &&
         lane_runtime->surface_resource_ids[ROAD_SIDE_RIGHT] !=
         ROAD_SURFACE_RESOURCE_MISSING)
        {
          return ROAD_CROSS_SECTION_UNAVAILABLE;
        }
      return lane_runtime->edge_depth_offsets[ROAD_SIDE_RIGHT];

    case ROAD_GEOMETRY_MODE_EDGE:
      return sample_edge_shape_height(lane_runtime, position);

    case ROAD_GEOMETRY_MODE_CURVED:
      if(lane_runtime->road_left < position)
        {
          return lane_runtime->edge_depth_offsets[ROAD_SIDE_RIGHT];
        }
      return lane_runtime->edge_depth_offsets[ROAD_SIDE_LEFT];

    case ROAD_GEOMETRY_MODE_PROFILE:
      fraction = (int)(interpolation_fraction &
                       ROAD_CROSS_SECTION_INTERPOLATION_MASK);
      next_lane = (const RoadSegmentLaneRuntime *)
                  lane_runtime->previous_links[1];
      if(position > interpolate_cross_section_value(
           lane_runtime->road_right, next_lane->road_right,
           fraction))
        {
          return sample_profile_right_height(
            lane_runtime, next_lane, position, fraction);
        }
      return sample_profile_left_height(
        lane_runtime, next_lane, position, fraction);

    case ROAD_GEOMETRY_MODE_EXCLUSION:
      if(position <
         lane_runtime->edge_shapes[ROAD_SIDE_RIGHT].inner_offset &&
         lane_runtime->edge_shapes[ROAD_SIDE_LEFT].inner_offset <
         position)
        {
          return 0;
        }
      return ROAD_CROSS_SECTION_UNAVAILABLE;
    }

  return 0;
}
