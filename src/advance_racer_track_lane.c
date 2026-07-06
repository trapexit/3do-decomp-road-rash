#include "track_segment_definition.h"

#include "ai_racer_runtime.h"
#include "road_segment_runtime.h"

enum RacerTrackLaneConstant
{
  RACER_TRACK_LANE_SIGN_SHIFT = 31,
  RACER_TRACK_LANE_HALF_SHIFT = 1,
  RACER_TRACK_LANE_RIDER_COLLISION_CLASS = 4
};

static
int
divide_by_two_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RACER_TRACK_LANE_SIGN_SHIFT)) >>
         RACER_TRACK_LANE_HALF_SHIFT;
}


static
RoadSegmentLaneRuntime *
select_connected_lane(RacerEntity            *racer,
                      RoadSegmentLaneRuntime *first,
                      RoadSegmentLaneRuntime *second)
{
  RoadSegmentLaneRuntime *selected;
  int channel;

  selected = first;
  channel = ROAD_TRACK_FIRST_CHANNEL;
  if(first != second &&
     first->road_right <=
     (racer->position_x >> TRACK_POSITION_FRACTION_BITS))
    {
      selected = second;
      channel = ROAD_TRACK_SECOND_CHANNEL;
    }

  if(first != second &&
     race_rider_state.player_object == racer)
    {
      select_road_track_channel_override(channel);
    }
  return selected;
}


void
advance_racer_track_lane(RacerEntity *racer,
                         int          track_delta)
{
  RoadSegmentLaneRuntime *current_lane;
  RoadSegmentLaneRuntime *first_lane;
  RoadSegmentLaneRuntime *second_lane;
  RoadSegmentLaneRuntime *selected_lane;
  AiRacerObject *ai_racer;
  int previous_position;
  int current_position;

  previous_position = racer->position_z;
  current_position = previous_position + track_delta;
  racer->position_z = current_position;
  racer->left_collision_bound = previous_position;
  racer->right_collision_bound = current_position;

  if((previous_position >> TRACK_POSITION_FRACTION_BITS) ==
     (current_position >> TRACK_POSITION_FRACTION_BITS))
    {
      return;
    }

  current_lane = racer->primary_track;
  racer->secondary_track = current_lane;
  if((previous_position >> TRACK_POSITION_FRACTION_BITS) <
     (current_position >> TRACK_POSITION_FRACTION_BITS))
    {
      first_lane = current_lane->previous_links[ROAD_TRACK_FIRST_CHANNEL];
      second_lane =
        current_lane->previous_links[ROAD_TRACK_SECOND_CHANNEL];
    }
  else
    {
      first_lane = current_lane->next_links[ROAD_TRACK_FIRST_CHANNEL];
      second_lane = current_lane->next_links[ROAD_TRACK_SECOND_CHANNEL];
    }

  selected_lane = select_connected_lane(racer, first_lane, second_lane);
  racer->primary_track = selected_lane;
  racer->track_segment = selected_lane;

  if(racer->collision_class ==
     RACER_TRACK_LANE_RIDER_COLLISION_CLASS)
    {
      ai_racer = (AiRacerObject *)racer;
      ai_racer->cursor_node =
        (AiTrackCursorNode *)selected_lane->clip_node;
      ai_racer->cursor_position = selected_lane->clip_position;
    }
}


static
void
apply_track_geometry_span(RacerEntity                  *racer,
                          const RoadSegmentLaneRuntime *lane,
                          int                           span)
{
  int heading_delta;

  heading_delta = span *
                  (lane->curvature_projection_step >>
                   TRACK_POSITION_FRACTION_BITS);
  racer->heading -= divide_by_two_toward_zero(heading_delta);
  racer->right_surface_bound = -(lane->path_curvature_step * span);
  racer->left_surface_bound = lane->path_elevation_step * span;
}


void
integrate_racer_track_geometry_step(RacerEntity *racer)
{
  RoadSegmentLaneRuntime *previous_lane;
  RoadSegmentLaneRuntime *current_lane;
  int previous_position;
  int current_position;
  int previous_span;
  int current_span;
  int value;

  previous_position = racer->left_collision_bound;
  current_position = racer->right_collision_bound;
  previous_lane = racer->secondary_track;
  current_lane = racer->primary_track;

  if((previous_position >> TRACK_POSITION_FRACTION_BITS) ==
     (current_position >> TRACK_POSITION_FRACTION_BITS))
    {
      apply_track_geometry_span(
        racer, current_lane, current_position - previous_position);
      return;
    }

  previous_span = previous_position & TRACK_POSITION_FRACTION_MASK;
  current_span = current_position & TRACK_POSITION_FRACTION_MASK;
  if(current_position < previous_position)
    {
      current_span -= TRACK_POSITION_UNIT;
      previous_span = -previous_span;
    }
  else
    {
      previous_span = TRACK_POSITION_UNIT - previous_span;
    }

  value = previous_span *
          (previous_lane->curvature_projection_step >>
           TRACK_POSITION_FRACTION_BITS);
  value += current_span *
           (current_lane->curvature_projection_step >>
            TRACK_POSITION_FRACTION_BITS);
  racer->heading -= divide_by_two_toward_zero(value);

  value = previous_span * previous_lane->path_curvature_step;
  value += current_span * current_lane->path_curvature_step;
  racer->right_surface_bound = -value;

  value = previous_span * previous_lane->path_elevation_step;
  value += current_span * current_lane->path_elevation_step;
  racer->left_surface_bound = value;
}
