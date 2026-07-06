#include "division_helpers.h"
#include "road_geometry.h"
#include "road_segment_runtime.h"
#include "track_segment_definition.h"

enum RoadLaneTransitionProfileValue
{
  ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT = 3,
  ROAD_LANE_TRANSITION_POINT_COUNT = 4,
  ROAD_LANE_TRANSITION_PROFILE_STRIDE = 2,
  ROAD_LANE_TRANSITION_DEPTH_CLEAR_THRESHOLD = 2,
  ROAD_LANE_TRANSITION_FRACTION_BITS = 16,
  ROAD_LANE_TRANSITION_FIXED_ONE =
    1 << ROAD_LANE_TRANSITION_FRACTION_BITS,
  ROAD_LANE_TRANSITION_HALF_SCALE =
    1 << (ROAD_LANE_TRANSITION_FRACTION_BITS - 1)
};

void
reconcile_road_lane_transition_profiles(RoadSegmentRuntime *segment)
{
  RoadSegmentLaneRuntime *left_lane;
  RoadSegmentLaneRuntime *right_lane;
  RoadTrackGraphNode *clearance_node;
  RoadLanePoint left_points[ROAD_LANE_TRANSITION_POINT_COUNT];
  RoadLanePoint right_points[ROAD_LANE_TRANSITION_POINT_COUNT];
  RoadLanePoint intersection;
  SignedDivisionResult division;
  int transition_amount;
  int geometry_mode;
  int transition_limit;
  int scale;
  int half_width;
  int left_index;
  int right_index;
  int attempts;
  int found;
  int index;
  int profile_index;
  int result;

  transition_amount = segment->transition_amount;
  left_lane = segment->selected_lanes[ROAD_TRACK_FIRST_CHANNEL];
  right_lane = segment->selected_lanes[ROAD_TRACK_SECOND_CHANNEL];
  geometry_mode = left_lane->geometry_mode;

  if(transition_amount > 0)
    {
      if(geometry_mode == ROAD_GEOMETRY_MODE_PROFILE ||
         transition_amount >
         ROAD_LANE_TRANSITION_DEPTH_CLEAR_THRESHOLD)
        {
          left_lane->edge_depth_offsets[ROAD_SIDE_RIGHT] = 0;
          right_lane->edge_depth_offsets[ROAD_SIDE_LEFT] = 0;
        }

      if(geometry_mode == ROAD_GEOMETRY_MODE_PROFILE)
        {
          for(index = 0;
              index < ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT;
              index++)
            {
              profile_index =
                index * ROAD_LANE_TRANSITION_PROFILE_STRIDE + 1;
              left_lane->surface_profile[profile_index].y = 0;
              profile_index =
                index * ROAD_LANE_TRANSITION_PROFILE_STRIDE;
              right_lane->surface_profile[profile_index].y = 0;
            }
        }

      clearance_node = left_lane->clip_node;
      if(segment->transition_type ==
         ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_SPAN ||
         segment->transition_type ==
         ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY)
        {
          clearance_node = clearance_node->primary_link;
        }
      else
        {
          clearance_node = clearance_node->secondary_link;
        }
      transition_limit =
        clearance_node->transition_runtime->branch_clearance_index;
      division = signed_divide_with_remainder(
        (unsigned int)transition_limit,
        (unsigned int)((transition_limit - transition_amount) *
                       ROAD_LANE_TRANSITION_FIXED_ONE));
      scale = division.quotient;
      if(scale < 0)
        {
          scale = 0;
        }

      if(left_lane->edge_margins[ROAD_SIDE_RIGHT] +
         right_lane->edge_margins[ROAD_SIDE_LEFT] >
         segment->left_step)
        {
          half_width = segment->left_step / ROAD_SIDE_COUNT;
          right_lane->edge_margins[ROAD_SIDE_LEFT] = half_width;
          left_lane->edge_margins[ROAD_SIDE_RIGHT] = half_width;
          if(scale < ROAD_LANE_TRANSITION_HALF_SCALE)
            {
              scale = ROAD_LANE_TRANSITION_HALF_SCALE;
            }
        }

      left_lane->edge_margins[ROAD_SIDE_RIGHT] =
        (scale * left_lane->edge_margins[ROAD_SIDE_RIGHT]) >>
        ROAD_LANE_TRANSITION_FRACTION_BITS;
      right_lane->edge_margins[ROAD_SIDE_LEFT] =
        (scale * right_lane->edge_margins[ROAD_SIDE_LEFT]) >>
        ROAD_LANE_TRANSITION_FRACTION_BITS;
      left_lane->shoulder_right =
        left_lane->road_right +
        left_lane->edge_margins[ROAD_SIDE_RIGHT];
      right_lane->shoulder_left =
        right_lane->road_left -
        right_lane->edge_margins[ROAD_SIDE_LEFT];
      return;
    }

  if(geometry_mode != ROAD_GEOMETRY_MODE_PROFILE)
    {
      return;
    }

  left_points[0].x = left_lane->shoulder_right;
  left_points[0].y = 0;
  right_points[0].x =
    right_lane->shoulder_left + segment->left_step;
  right_points[0].y = 0;
  for(index = 0;
      index < ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT;
      index++)
    {
      profile_index =
        index * ROAD_LANE_TRANSITION_PROFILE_STRIDE + 1;
      left_points[index + 1].x =
        left_lane->surface_profile[profile_index].x;
      left_points[index + 1].y =
        left_lane->surface_profile[profile_index].y;

      profile_index = index * ROAD_LANE_TRANSITION_PROFILE_STRIDE;
      right_points[index + 1].x =
        right_lane->surface_profile[profile_index].x +
        segment->left_step;
      right_points[index + 1].y =
        right_lane->surface_profile[profile_index].y;
    }

  left_index = 1;
  right_index = 1;
  attempts = 1;
  found = 0;
  do
    {
      if(left_points[left_index].x < right_points[right_index].x &&
         left_index < ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT)
        {
          left_index++;
        }
      found =
        left_points[left_index].x >= right_points[right_index].x;
      if(found == 0 &&
         right_index < ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT)
        {
          right_index++;
        }
      attempts++;
    } while(attempts < ROAD_LANE_TRANSITION_POINT_COUNT && found == 0);

  if(found == 0)
    {
      found = left_points[left_index].x >= right_points[right_index].x;
    }

  if(found != 0)
    {
      result = calculate_road_profile_intersection(
        &left_points[left_index - 1],
        &left_points[left_index],
        &right_points[right_index - 1],
        &right_points[right_index],
        &intersection);
      if(result == 0)
        {
          intersection.x =
            (left_points[left_index].x +
             right_points[right_index].x) / ROAD_SIDE_COUNT;
          intersection.y =
            (left_points[left_index].y +
             right_points[right_index].y) / ROAD_SIDE_COUNT;
        }
      if(transition_amount == 0)
        {
          intersection.y = 0;
        }
      for(index = left_index;
          index < ROAD_LANE_TRANSITION_POINT_COUNT;
          index++)
        {
          left_points[index].x = intersection.x;
          left_points[index].y = intersection.y;
        }
      for(index = right_index;
          index < ROAD_LANE_TRANSITION_POINT_COUNT;
          index++)
        {
          right_points[index].x = intersection.x;
          right_points[index].y = intersection.y;
        }
    }
  else
    {
      left_index = ROAD_LANE_TRANSITION_POINT_COUNT;
      right_index = ROAD_LANE_TRANSITION_POINT_COUNT;
    }

  segment->left_transition_profile_count = (unsigned char)left_index;
  segment->right_transition_profile_count = (unsigned char)right_index;
  for(index = 0;
      index < ROAD_LANE_TRANSITION_CONTROL_POINT_COUNT;
      index++)
    {
      profile_index =
        index * ROAD_LANE_TRANSITION_PROFILE_STRIDE + 1;
      left_lane->surface_profile[profile_index].x =
        left_points[index + 1].x;
      left_lane->surface_profile[profile_index].y =
        left_points[index + 1].y;

      profile_index = index * ROAD_LANE_TRANSITION_PROFILE_STRIDE;
      right_lane->surface_profile[profile_index].x =
        right_points[index + 1].x - segment->left_step;
      right_lane->surface_profile[profile_index].y =
        right_points[index + 1].y;
    }
}
