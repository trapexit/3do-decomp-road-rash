#include "bss_late_data.h"
#include "division_helpers.h"
#include "racer_runtime.h"
#include "road_geometry.h"

enum BoundaryCollisionKind14998
{
  BOUNDARY_COLLISION_KIND_5 = 5
};

enum BoundaryGeometryMode14998
{
  BOUNDARY_GEOMETRY_MODE_5 = 5
};

enum BoundaryResponse14998
{
  BOUNDARY_RESPONSE_NONE = 0,
  BOUNDARY_RESPONSE_BOUNCE = 1,
  BOUNDARY_RESPONSE_SOFT_BOUNCE = 2,
  BOUNDARY_RESPONSE_RECOVERY = 3
};

enum BoundaryCollisionClass14998
{
  BOUNDARY_COLLISION_CLASS_RIDER = 4,
  BOUNDARY_COLLISION_CLASS_PAIRED_RIDER = 8
};

enum BoundaryRiderMode14998
{
  BOUNDARY_RIDER_MODE_NORMAL = 0,
  BOUNDARY_RIDER_MODE_RECOVERY = 1
};

enum BoundaryEdgeFlag14998
{
  BOUNDARY_EDGE_LEFT = 1,
  BOUNDARY_EDGE_RIGHT = 2,
  BOUNDARY_EDGE_HEIGHT_MASK = 3
};

enum BoundarySurfaceFlag14998
{
  BOUNDARY_SURFACE_COLLISION_DISABLED_MASK = 0x38,
  BOUNDARY_SURFACE_SLOW_TURN_DISABLED = 0x40
};

enum BoundaryRecoveryKind14998
{
  BOUNDARY_RECOVERY_HEIGHT_LIMIT = 3,
  BOUNDARY_RECOVERY_FAST_IMPACT = 6
};

enum BoundaryFixedValue14998
{
  BOUNDARY_POSITION_SHIFT = 8,
  BOUNDARY_SIGN_BIT_SHIFT = 31,
  BOUNDARY_POSITION_ONE = 0x100,
  BOUNDARY_POSITION_FRACTION_MASK = 0xFF,
  BOUNDARY_ROAD_SAMPLE_SPACING = 0xFA,
  BOUNDARY_MARGIN_DIVISOR = 3,
  BOUNDARY_EDGE_CLEARANCE = 4,
  BOUNDARY_DEFAULT_HEIGHT_LIMIT = 0x40,
  BOUNDARY_BOUNCE_NUMERATOR = 100,
  BOUNDARY_FAST_IMPACT_SPEED = 0x9C4,
  BOUNDARY_PAIRED_VERTICAL_IMPULSE = 0x50,
  BOUNDARY_RIDER_VERTICAL_IMPULSE = 0x10,
  BOUNDARY_REVERSE_HEADING = 0x800000
};

void
advance_racer_track_position(RacerEntity *racer,
                             int          track_delta,
                             int          accumulate_contact);
void
begin_rider_recovery(RacerEntity         *rider,
                     const RacerVelocity *state_velocity_delta,
                     const RacerVelocity *rider_velocity_delta,
                     int                  recovery_kind);

__inline
static
int
div256_round_towards_zero(int value)
{
  if(value < 0)
    {
      value += BOUNDARY_POSITION_FRACTION_MASK;
    }
  return value >> BOUNDARY_POSITION_SHIFT;
}


__inline
static
int
fixed_to_cell(int value)
{
  return value >> BOUNDARY_POSITION_SHIFT;
}


__inline
static
int
half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> BOUNDARY_SIGN_BIT_SHIFT)) >> 1;
}


__inline
static
int
absolute_value(int value)
{
  if(value < 0)
    {
      return -value;
    }
  return value;
}


__inline
static
int
signed_quotient(int divisor,
                      int numerator)
{
  SignedDivisionResult result;

  result = signed_divide_with_remainder((unsigned int)divisor,
                                        (unsigned int)numerator);
  return result.quotient;
}


__inline
static
int
interpolate_byte_fraction(int base,
                                int next,
                                int fixed_position)
{
  int delta;

  delta = next - base;
  if(delta != 0)
    {
      delta *= fixed_position & BOUNDARY_POSITION_FRACTION_MASK;
      if(delta < 0)
        {
          delta += BOUNDARY_POSITION_FRACTION_MASK;
        }
      base += delta >> BOUNDARY_POSITION_SHIFT;
    }
  return base;
}


__inline
static
int
interpolated_shoulder(RoadSegmentLaneRuntime *lane,
                            RoadSide                side,
                            int                     fixed_position)
{
  RoadSegmentLaneRuntime *linked_lane;
  int base;
  int next;

  linked_lane = (RoadSegmentLaneRuntime *)lane->previous_links[side];
  if(side == ROAD_SIDE_LEFT)
    {
      base = lane->shoulder_left;
      next = linked_lane->shoulder_left;
    }
  else
    {
      base = lane->shoulder_right;
      next = linked_lane->shoulder_right;
    }

  return interpolate_byte_fraction(base, next, fixed_position);
}


__inline
static
int
lane_height_limit(unsigned int edge_flags)
{
  int encoded_height;

  encoded_height = (int)(edge_flags & ~BOUNDARY_EDGE_HEIGHT_MASK);
  if(encoded_height == 0)
    {
      return BOUNDARY_DEFAULT_HEIGHT_LIMIT;
    }
  return encoded_height << BOUNDARY_POSITION_SHIFT;
}


__inline
static
int
target_inside_right_boundary(int boundary,
                                   int half_width)
{
  return boundary * BOUNDARY_POSITION_ONE - half_width;
}


__inline
static
int
target_inside_left_boundary(int boundary,
                                  int half_width)
{
  return half_width + boundary * BOUNDARY_POSITION_ONE;
}


__inline
static
void
begin_fast_boundary_recovery(RacerEntity *racer)
{
  RacerVelocity state_velocity_delta;
  RacerVelocity rider_velocity_delta;

  state_velocity_delta.forward = 0;
  state_velocity_delta.lateral = 0;
  state_velocity_delta.vertical = BOUNDARY_PAIRED_VERTICAL_IMPULSE;
  rider_velocity_delta.forward = 0;
  rider_velocity_delta.lateral = 0;
  rider_velocity_delta.vertical = BOUNDARY_RIDER_VERTICAL_IMPULSE;

  begin_rider_recovery(racer, &state_velocity_delta, &rider_velocity_delta,
                       BOUNDARY_RECOVERY_FAST_IMPACT);
}


void
resolve_racer_road_boundary_collision(RacerEntity *racer)
{
  RoadSegmentLaneRuntime *lane;
  RoadSegmentLaneRuntime *previous_lane;
  RacerEntity *player_rider;
  RacerEntity *paired_player_rider;
  int half_width;
  int x_min;
  int x_max;
  int geometry_mode;
  int collision_class;
  int response;
  int slow_turn;
  int target_x;
  unsigned int edge_flags;
  int height_limit;
  int boundary;
  int previous_boundary;
  int overlap;
  RoadSide side;

  half_width = racer->collision_half_width_x;
  x_min = fixed_to_cell(racer->position_x - half_width);
  x_max = fixed_to_cell(racer->position_x + half_width);
  lane = (RoadSegmentLaneRuntime *)racer->track_segment;

  if(lane->road_left < x_min && x_max < lane->road_right)
    {
      return;
    }

  response = BOUNDARY_RESPONSE_NONE;
  slow_turn = 0;
  target_x = racer->position_x;

  if(lane->collision_kind == BOUNDARY_COLLISION_KIND_5)
    {
      collision_class = racer->collision_class;
      if(collision_class != BOUNDARY_COLLISION_CLASS_RIDER)
        {
          if(collision_class != BOUNDARY_COLLISION_CLASS_PAIRED_RIDER ||
             racer->owner->rider_mode != BOUNDARY_RIDER_MODE_RECOVERY)
            {
              goto normal_collision;
            }
        }

      edge_flags = lane->edge_collision_flags;
      height_limit = lane_height_limit(edge_flags);
      if(height_limit <= racer->position_y)
        {
          goto normal_collision;
        }

      if((edge_flags & BOUNDARY_EDGE_LEFT) != 0)
        {
          boundary = interpolated_shoulder(
            lane, ROAD_SIDE_LEFT, racer->position_z);
          boundary -= lane->collision_inset;
          if(x_min < boundary && boundary < x_max)
            {
              response = BOUNDARY_RESPONSE_BOUNCE;
              previous_lane = (RoadSegmentLaneRuntime *)
                              racer->previous_track_segment;
              previous_boundary = interpolated_shoulder(
                previous_lane, ROAD_SIDE_LEFT,
                racer->previous_position_z);
              previous_boundary -= previous_lane->collision_inset;
              if(racer->previous_position_x <
                 previous_boundary * BOUNDARY_POSITION_ONE)
                {
                  target_x = target_inside_right_boundary(
                    boundary - BOUNDARY_EDGE_CLEARANCE, half_width);
                }
              else
                {
                  target_x = target_inside_left_boundary(
                    boundary + BOUNDARY_EDGE_CLEARANCE, half_width);
                }
            }
        }

      if(response == BOUNDARY_RESPONSE_NONE &&
         (edge_flags & BOUNDARY_EDGE_RIGHT) != 0)
        {
          boundary = interpolated_shoulder(
            lane, ROAD_SIDE_RIGHT, racer->position_z);
          boundary += lane->collision_inset;
          if(x_min < boundary && boundary < x_max)
            {
              response = BOUNDARY_RESPONSE_BOUNCE;
              previous_lane = (RoadSegmentLaneRuntime *)
                              racer->previous_track_segment;
              previous_boundary = interpolated_shoulder(
                previous_lane, ROAD_SIDE_RIGHT,
                racer->previous_position_z);
              previous_boundary += previous_lane->collision_inset;
              if(racer->previous_position_x >
                 previous_boundary * BOUNDARY_POSITION_ONE)
                {
                  target_x = target_inside_left_boundary(
                    boundary + BOUNDARY_EDGE_CLEARANCE, half_width);
                }
              else
                {
                  target_x = target_inside_right_boundary(
                    boundary - BOUNDARY_EDGE_CLEARANCE, half_width);
                }
            }
        }

      if(response == BOUNDARY_RESPONSE_NONE)
        {
          goto normal_collision;
        }

      if(racer->surface_contact_scale_8_8 == 0 &&
         racer->position_y + racer->collision_half_height_y >
         height_limit)
        {
          if(collision_class == BOUNDARY_COLLISION_CLASS_RIDER)
            {
              if(racer->rider_mode == BOUNDARY_RIDER_MODE_NORMAL)
                {
                  response = BOUNDARY_RESPONSE_RECOVERY;
                  goto finish_response;
                }
            }
          else if(collision_class ==
                  BOUNDARY_COLLISION_CLASS_PAIRED_RIDER)
            {
              return;
            }
        }

      if(response == BOUNDARY_RESPONSE_NONE)
        {
          return;
        }
      goto finish_response;
    }

normal_collision:
  geometry_mode = lane->geometry_mode;
  if(geometry_mode != ROAD_GEOMETRY_MODE_TEXTURED)
    {
      if(geometry_mode == BOUNDARY_GEOMETRY_MODE_5)
        {
          boundary = lane->edge_shapes[ROAD_SIDE_RIGHT].inner_offset;
          if(boundary < x_max)
            {
              target_x = target_inside_right_boundary(
                boundary, half_width);
              response = BOUNDARY_RESPONSE_BOUNCE;
            }
          else
            {
              boundary = lane->edge_shapes[ROAD_SIDE_LEFT].inner_offset;
              if(boundary <= x_min)
                {
                  return;
                }
              target_x = target_inside_left_boundary(
                boundary, half_width);
              response = BOUNDARY_RESPONSE_BOUNCE;
            }
          goto finish_response;
        }

      if(geometry_mode == ROAD_GEOMETRY_MODE_EDGE)
        {
          player_rider = (RacerEntity *)race_rider_state.player_object;
          paired_player_rider = player_rider;
          if(racer != player_rider)
            {
              paired_player_rider = player_rider->paired_state;
            }

          if(racer == player_rider || racer == paired_player_rider)
            {
              boundary =
                lane->edge_shapes[ROAD_SIDE_RIGHT].inner_offset;
              if(boundary < x_max)
                {
                  if(lane->edge_shapes[ROAD_SIDE_RIGHT].height > 0)
                    {
                      target_x = target_inside_right_boundary(
                        boundary, half_width);
                      response = BOUNDARY_RESPONSE_BOUNCE;
                    }
                  else
                    {
                      boundary =
                        lane->edge_shapes[ROAD_SIDE_RIGHT].outer_offset;
                      if(x_max <= boundary)
                        {
                          return;
                        }
                      target_x = target_inside_right_boundary(
                        boundary, half_width);
                      response = BOUNDARY_RESPONSE_BOUNCE;
                    }
                }
              else
                {
                  boundary =
                    lane->edge_shapes[ROAD_SIDE_LEFT].inner_offset;
                  if(boundary <= x_min)
                    {
                      return;
                    }
                  if(lane->edge_shapes[ROAD_SIDE_LEFT].height > 0)
                    {
                      target_x = target_inside_left_boundary(
                        boundary, half_width);
                      response = BOUNDARY_RESPONSE_BOUNCE;
                    }
                  else
                    {
                      boundary =
                        lane->edge_shapes[ROAD_SIDE_LEFT].outer_offset;
                      if(boundary <= x_min)
                        {
                          return;
                        }
                      target_x = target_inside_left_boundary(
                        boundary, half_width);
                      response = BOUNDARY_RESPONSE_BOUNCE;
                    }
                }
            }
          else
            {
              boundary = lane->shoulder_right;
              if(boundary < x_max)
                {
                  target_x = target_inside_right_boundary(
                    boundary, half_width);
                  response = BOUNDARY_RESPONSE_BOUNCE;
                }
              else
                {
                  boundary = lane->shoulder_left;
                  if(boundary <= x_min)
                    {
                      return;
                    }
                  target_x = target_inside_left_boundary(
                    boundary, half_width);
                  response = BOUNDARY_RESPONSE_BOUNCE;
                }
            }
          goto finish_response;
        }

      if(geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
        {
          boundary = lane->shoulder_right;
          if(boundary < x_max)
            {
              target_x = target_inside_right_boundary(
                boundary, half_width);
              overlap = x_max - boundary;
            }
          else
            {
              boundary = lane->shoulder_left;
              if(boundary <= x_min)
                {
                  return;
                }
              target_x = target_inside_left_boundary(
                boundary, half_width);
              overlap = boundary - x_min;
            }
          response = BOUNDARY_RESPONSE_BOUNCE;
          previous_lane = (RoadSegmentLaneRuntime *)
                          racer->previous_track_segment;
          if((previous_lane->surface_flags &
              BOUNDARY_SURFACE_SLOW_TURN_DISABLED) == 0 &&
             overlap > fixed_to_cell(
               half_towards_zero(half_width)))
            {
              slow_turn = 1;
              if(overlap < fixed_to_cell(
                   half_width +
                   half_towards_zero(half_width)))
                {
                  response = BOUNDARY_RESPONSE_SOFT_BOUNCE;
                }
              goto handle_slow_turn;
            }
          goto finish_response;
        }

      return;
    }

  if((lane->surface_flags &
      BOUNDARY_SURFACE_COLLISION_DISABLED_MASK) != 0)
    {
      return;
    }

  boundary = lane->shoulder_right;
  if(lane->surface_resource_ids[ROAD_SIDE_RIGHT] ==
     ROAD_SURFACE_RESOURCE_MISSING)
    {
      boundary += signed_quotient(
        BOUNDARY_MARGIN_DIVISOR,
        (racer->position_z &
         BOUNDARY_POSITION_FRACTION_MASK) *
        BOUNDARY_ROAD_SAMPLE_SPACING) >>
                  BOUNDARY_POSITION_SHIFT;
    }

  if(boundary < x_max)
    {
      target_x = target_inside_right_boundary(boundary, half_width);
      side = ROAD_SIDE_RIGHT;
      overlap = x_max - boundary;
    }
  else
    {
      boundary = lane->shoulder_left;
      if(lane->surface_resource_ids[ROAD_SIDE_LEFT] ==
         ROAD_SURFACE_RESOURCE_MISSING)
        {
          boundary -= signed_quotient(
            BOUNDARY_MARGIN_DIVISOR,
            (racer->position_z &
             BOUNDARY_POSITION_FRACTION_MASK) *
            BOUNDARY_ROAD_SAMPLE_SPACING) >>
                      BOUNDARY_POSITION_SHIFT;
        }
      if(boundary <= x_min)
        {
          return;
        }
      target_x = target_inside_left_boundary(boundary, half_width);
      side = ROAD_SIDE_LEFT;
      overlap = boundary - x_min;
    }

  response = BOUNDARY_RESPONSE_BOUNCE;
  previous_lane = (RoadSegmentLaneRuntime *)racer->previous_track_segment;
  if(((previous_lane->surface_flags &
       ROAD_SURFACE_FLAG_MARGIN_MASK) == 0 &&
      (lane->surface_resource_ids[side] ==
       ROAD_SURFACE_RESOURCE_MISSING ||
       previous_lane->surface_resource_ids[side] !=
       ROAD_SURFACE_RESOURCE_MISSING)) ||
     overlap <= fixed_to_cell(
       half_width + half_towards_zero(half_width)))
    {
      goto finish_response;
    }

  slow_turn = 1;
  if(overlap < fixed_to_cell(half_width << 1))
    {
      response = BOUNDARY_RESPONSE_SOFT_BOUNCE;
    }

handle_slow_turn:
  if(absolute_value(racer->velocity.forward) >=
     BOUNDARY_FAST_IMPACT_SPEED)
    {
      if(response != BOUNDARY_RESPONSE_SOFT_BOUNCE)
        {
          racer->velocity.forward = 0;
          advance_racer_track_position(racer,
                                       racer->previous_position_z - racer->position_z,
                                       0);
        }
    }
  else
    {
      slow_turn = 0;
    }

  if(slow_turn == 0)
    {
      goto check_lateral_speed;
    }

  if(racer->collision_class == BOUNDARY_COLLISION_CLASS_RIDER)
    {
      begin_fast_boundary_recovery(racer);
    }
  racer->position_x = target_x;
  return;

finish_response:
  if(response == BOUNDARY_RESPONSE_BOUNCE ||
     response == BOUNDARY_RESPONSE_SOFT_BOUNCE)
    {
      racer->velocity.lateral = -div256_round_towards_zero(
        racer->velocity.lateral * BOUNDARY_BOUNCE_NUMERATOR);
    }
  else if(response == BOUNDARY_RESPONSE_RECOVERY &&
          racer->collision_class ==
          BOUNDARY_COLLISION_CLASS_RIDER)
    {
      begin_rider_recovery(racer, 0, 0, BOUNDARY_RECOVERY_HEIGHT_LIMIT);
      if(racer->position_y + racer->collision_half_height_y <
         lane_height_limit(lane->edge_collision_flags))
        {
          racer->velocity.lateral = -div256_round_towards_zero(
            racer->velocity.lateral * BOUNDARY_BOUNCE_NUMERATOR);
        }
      else
        {
          target_x = racer->position_x;
        }
    }

  racer->orientation.steering_heading = 0;
  collision_class = racer->collision_class;
  if(collision_class == BOUNDARY_COLLISION_CLASS_RIDER)
    {
      if(racer->velocity.lateral > 0)
        {
          if(racer->steering_angle > 0)
            {
              racer->steering_angle = 0;
            }
        }
      else if(racer->velocity.lateral < 0 &&
              racer->steering_angle < 0)
        {
          racer->steering_angle = 0;
        }
      racer->orientation.movement_heading = 0;
    }
  else if(collision_class ==
          BOUNDARY_COLLISION_CLASS_PAIRED_RIDER)
    {
      if(racer->world_velocity.forward < 0)
        {
          racer->orientation.movement_heading =
            BOUNDARY_REVERSE_HEADING;
        }
      else
        {
          racer->orientation.movement_heading = 0;
        }
    }
  else
    {
      racer->orientation.movement_heading = 0;
    }

check_lateral_speed:
  if(slow_turn == 0 &&
     absolute_value(racer->velocity.lateral) >=
     BOUNDARY_FAST_IMPACT_SPEED &&
     response != BOUNDARY_RESPONSE_RECOVERY)
    {
      if(racer->collision_class == BOUNDARY_COLLISION_CLASS_RIDER)
        {
          begin_fast_boundary_recovery(racer);
        }
    }
  racer->position_x = target_x;
}
