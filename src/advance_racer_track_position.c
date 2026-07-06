#include "racer_runtime.h"
#include "track_traversal_runtime.h"

enum RacerTrackPositionConstant
{
  RACER_TRACK_GEOMETRY_UPDATE_MASK = 0xFF
};

void
advance_racer_track_lane(RacerEntity *racer,
                         int          track_delta);
void
integrate_racer_track_geometry_step(RacerEntity *racer);

static
int
absolute_track_delta(int value)
{
  if(value < 0)
    {
      return -value;
    }
  return value;
}


void
advance_racer_track_position_slice(RacerEntity *racer,
                                   int          track_delta,
                                   int          update_track_geometry)
{
  advance_racer_track_lane(racer, track_delta);
  if((update_track_geometry & RACER_TRACK_GEOMETRY_UPDATE_MASK) != 0)
    {
      integrate_racer_track_geometry_step(racer);
    }
}


void
advance_racer_track_position(RacerEntity *racer,
                             int          track_delta,
                             int          update_track_geometry)
{
  RoadSegmentLaneRuntime *previous_lane;
  int previous_position;
  int crossed_cell_count;
  int cell_step;
  int remaining_delta;
  int first_span;
  int accumulated_elevation_step;
  int accumulated_curvature_step;
  int update_geometry;
  int cell_index;

  update_geometry =
    update_track_geometry & RACER_TRACK_GEOMETRY_UPDATE_MASK;
  if(track_delta == 0)
    {
      if(update_geometry != 0)
        {
          racer->right_surface_bound = 0;
          racer->left_surface_bound = 0;
        }
      return;
    }

  previous_position = racer->position_z;
  crossed_cell_count =
    ((previous_position + track_delta) >>
     TRACK_POSITION_FRACTION_BITS) -
    (previous_position >> TRACK_POSITION_FRACTION_BITS);
  cell_step = TRACK_POSITION_UNIT;
  if(crossed_cell_count < 0)
    {
      cell_step = -TRACK_POSITION_UNIT;
      crossed_cell_count = -crossed_cell_count;
    }

  if(crossed_cell_count <= 1)
    {
      advance_racer_track_position_slice(
        racer, track_delta, update_geometry);
      return;
    }

  previous_lane = racer->track_segment;
  remaining_delta = absolute_track_delta(track_delta);
  first_span = remaining_delta & TRACK_POSITION_FRACTION_MASK;
  if(cell_step < 0)
    {
      first_span = -first_span;
    }
  advance_racer_track_position_slice(
    racer, first_span, update_geometry);

  accumulated_elevation_step = racer->left_surface_bound;
  accumulated_curvature_step = racer->right_surface_bound;
  cell_index = 0;
  while(cell_index <
        (remaining_delta >> TRACK_POSITION_FRACTION_BITS))
    {
      advance_racer_track_position_slice(
        racer, cell_step, update_geometry);
      accumulated_curvature_step += racer->right_surface_bound;
      accumulated_elevation_step += racer->left_surface_bound;
      cell_index++;
    }

  racer->left_collision_bound = previous_position;
  racer->secondary_track = previous_lane;
  if(update_geometry != 0)
    {
      racer->left_surface_bound = accumulated_elevation_step;
      racer->right_surface_bound = accumulated_curvature_step;
    }
}
