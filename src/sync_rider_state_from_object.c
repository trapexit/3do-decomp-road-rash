#include "racer_runtime.h"

void
sync_rider_state_from_object(RacerEntity *rider)
{
  RacerEntity *state;

  if(rider->rider_mode != 0)
    {
      return;
    }

  state = rider->paired_state;

  state->previous_position_z = rider->previous_position_z;
  state->previous_position_y = rider->previous_position_y;
  state->previous_position_aux = rider->previous_position_aux;
  state->previous_position_x = rider->previous_position_x;
  state->previous_track_segment = rider->previous_track_segment;

  state->position_z = rider->position_z;
  state->position_y = rider->position_y;
  state->position_aux = rider->position_aux;
  state->position_x = rider->position_x;
  state->track_segment = rider->track_segment;

  state->secondary_track = rider->secondary_track;
  state->primary_track = rider->primary_track;
  state->left_collision_bound = rider->left_collision_bound;
  state->right_collision_bound = rider->right_collision_bound;

  state->orientation = rider->orientation;
  state->velocity = rider->velocity;
  state->world_velocity = rider->world_velocity;
}
