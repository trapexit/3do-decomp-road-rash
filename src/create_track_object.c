#include "cans_animation_binding.h"
#include "rider_animation_runtime.h"
#include "rw_semantic_data.h"
#include "stdlib.h"
#include "track_object_query.h"
#include "track_object_runtime.h"

typedef struct DivModResult
{
  unsigned int quotient;
  unsigned int remainder;
} DivModResult;

__value_in_regs DivModResult
signed_divide_with_remainder(unsigned int divisor,
                             unsigned int numerator);
int
absolute_int_difference(int first,
                        int second);
int
measure_track_object_distance(const TrackObjectPosition *current_position,
                              const TrackObjectPosition *target_position,
                              int                        mode);
static
int
as_signed_byte(int value)
{
  return (signed char)value;
}


static
int
divide_by_two_toward_zero(int value)
{
  return (value + (int)((unsigned int)value >>
                        (sizeof(unsigned int) *
                         TRACK_OBJECT_BITS_PER_BYTE - 1))) >> 1;
}


static
int
divide_by_position_unit_toward_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_OBJECT_POSITION_UNIT - 1;
    }
  return value >> TRACK_OBJECT_POSITION_FRACTION_BITS;
}


static
int
scale_track_velocity(int value)
{
  return (value >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_A) +
         (value >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_B) +
         (value >> TRACK_OBJECT_VELOCITY_SCALE_SHIFT_C);
}


static
int
as_signed_24_bit(int value)
{
  return (value << TRACK_OBJECT_SIGNED_24_SHIFT) >>
         TRACK_OBJECT_SIGNED_24_SHIFT;
}


static
int
track_lane_is_valid(RoadSegmentLaneRuntime *road,
                    int                     mode,
                    int                     lane)
{
  int lane_count;

  if(mode == TRACK_OBJECT_MODE_FORWARD)
    {
      lane_count = divide_by_position_unit_toward_zero(
        road->right_width);
      return lane >= 0 && lane < lane_count;
    }

  lane_count = divide_by_position_unit_toward_zero(
    road->left_width);
  return lane < 0 && -lane <= lane_count;
}


static
RoadSegmentLaneRuntime *
select_adjacent_road_segment(RoadSegmentLaneRuntime *road,
                             int                     mode)
{
  RoadJunctionRuntime *junction;
  int direction;

  junction = (RoadJunctionRuntime *)road->resource_handle;
  direction = junction->connection_direction;
  if(mode == TRACK_OBJECT_MODE_FORWARD)
    {
      if(direction != TRACK_ROAD_FORWARD_CONNECTION_DIRECTION)
        {
          road = (RoadSegmentLaneRuntime *)road->previous_links[0];
        }
    }
  else if(direction != TRACK_ROAD_REVERSE_CONNECTION_DIRECTION)
    {
      road = (RoadSegmentLaneRuntime *)road->next_links[0];
    }
  return road;
}


static
int
animation_state_is_available(RiderAnimationRuntime *animation,
                             int                    animation_state)
{
  return *(int *)animation->state_table[animation_state] != -1;
}


void
create_track_object(int                     mode,
                    int                     lane,
                    int                     offset,
                    TrackInteractionObject *parent,
                    RoadSegmentLaneRuntime *track)
{
  signed char candidates[TRACK_OBJECT_ANIMATION_CANDIDATE_CAPACITY];
  TrackObject *object;
  TrackObjectModeInitialization *template_fields;
  const TrackObjectModeInitialization *mode_fields;
  unsigned int animation_mask;
  int position;
  int orientation;
  int candidate_count;
  int animation_index;
  int animation_flags;
  int last_animation_index;
  DivModResult divmod;

  if(runtime_feature_flags.track_objects_enabled.value == 0)
    {
      return;
    }

  if((mode & TRACK_OBJECT_MODE_DIRECTION_BIT) != 0)
    {
      position = track->center_position + offset;
      orientation = TRACK_OBJECT_PERPENDICULAR_ORIENTATION;
      animation_mask = TRACK_OBJECT_CROSS_ANIMATION_FLAG;
    }
  else
    {
      position = (track->center_position +
                  lane * TRACK_OBJECT_LANE_SPACING) *
                 TRACK_OBJECT_POSITION_UNIT +
                 TRACK_OBJECT_LANE_CENTER_BIAS;
      orientation = 0;
      if(mode == TRACK_OBJECT_MODE_REVERSE)
        {
          animation_mask = TRACK_OBJECT_REVERSE_ANIMATION_FLAG;
        }
      else
        {
          animation_mask = TRACK_OBJECT_FORWARD_ANIMATION_FLAG;
        }
    }

  candidate_count = 0;
  last_animation_index = car_animation_load_state.animation_count - 1;
  animation_index = 0;
  while(animation_index < last_animation_index)
    {
      animation_flags = get_car_animation_output_flags(animation_index);
      if((animation_flags & TRACK_OBJECT_DIRECTION_ANIMATION_FLAGS) == 0 ||
         ((unsigned int)animation_flags & animation_mask) != 0)
        {
          candidates[candidate_count] = (signed char)animation_index;
          candidate_count = as_signed_byte(candidate_count + 1);
        }
      animation_index = as_signed_byte(animation_index + 1);
    }

  if(candidate_count <= 0)
    {
      return;
    }

  divmod = signed_divide_with_remainder((unsigned int)candidate_count,
                                        (unsigned int)rand());
  animation_index = candidates[divmod.remainder];

  template_fields = (TrackObjectModeInitialization *)
                    &track_object_template.collision_half_width_x;
  mode_fields = (const TrackObjectModeInitialization *)
                &track_object_mode_templates[mode];
  *template_fields = *mode_fields;

  object = (TrackObject *)create_racer_entity_from_template(
    track_simulation_globals.active_object, &track_object_template,
    position, orientation, track);
  if(object == 0)
    {
      return;
    }

  object->cruise_speed =
    (rand() & TRACK_OBJECT_RANDOM_SPEED_MASK) +
    TRACK_OBJECT_RANDOM_SPEED_BASE;
  object->animation_phase = (unsigned char)rand();
  object->animation_color = (unsigned char)rand();
  object->animation_variant = (unsigned char)rand();
  object->mode = mode;

  if(parent != 0)
    {
      object->inherited_velocity = parent->motion_source->world_velocity;
    }
  else
    {
      object->inherited_velocity.forward = 0;
      object->inherited_velocity.lateral = 0;
      object->inherited_velocity.vertical = 0;
    }

  object->motion_command.lateral_offset = 0;
  object->motion_command.direction = 0;
  object->lane = (signed char)lane;
  object->target = parent;
  object->nearby_object = 0;
  object->mode_update_seen = 0;
  object->delay_ticks = 0;

  initialize_car_animation(&object->animation, animation_index);
  track_object_remains_active(object);
}


void
track_object_update(TrackObject *object)
{
  TrackInteractionObject *candidate;
  TrackInteractionObject *target;
  RoadSegmentLaneRuntime *road;
  RiderAnimationRuntime *animation;
  int mode;
  int lane;
  int speed;
  int base_speed;
  int road_speed;
  int road_margin;
  int width;
  int double_width;
  int distance;
  int coordinate;
  int target_delta;
  int target_offset;
  int desired_offset;
  int limit;
  int projected;
  int required;
  int collision_adjusted;
  int lane_changed;
  int boost;
  int acceleration_scale;

  collision_adjusted = 0;
  lane_changed = 0;
  boost = 0;

  if(track_object_remains_active(object) == 0)
    {
      return;
    }

  if(object->delay_ticks != 0)
    {
      object->delay_ticks -= object->time_step;
      if(object->delay_ticks < 0)
        {
          object->delay_ticks = 0;
        }
    }

  mode = object->mode;
  if((mode & TRACK_OBJECT_MODE_DIRECTION_BIT) != 0)
    {
      unsigned char visible;
      unsigned int relative;

      relative = (unsigned int)(
        object->position.track_position -
        (track_runtime_globals.current_track_position -
         TRACK_OBJECT_VISIBILITY_LOOKBEHIND));
      visible = object->status_flags;
      if(relative <= TRACK_OBJECT_VISIBILITY_DISTANCE)
        {
          visible |= TRACK_OBJECT_VISIBILITY_BITS;
        }
      else
        {
          visible &= (unsigned char)~TRACK_OBJECT_VISIBILITY_BITS;
        }
      object->status_flags = visible;
    }

  base_speed = (object->cruise_speed *
                track_simulation_globals.packed_divisor) >>
               TRACK_OBJECT_POSITION_FRACTION_BITS;
  speed = base_speed;
  road_speed = scale_track_velocity(object->velocity.forward);
  road_margin = as_signed_24_bit(
    road_speed * TRACK_OBJECT_ROAD_MARGIN_MULTIPLIER);
  width = road_margin;
  if(width <= TRACK_OBJECT_MINIMUM_WIDTH)
    {
      width = TRACK_OBJECT_MINIMUM_WIDTH;
    }
  double_width = width * TRACK_OBJECT_WIDTH_MULTIPLIER;
  distance = TRACK_OBJECT_DEFAULT_SEARCH_DISTANCE;

  target = object->target;
  if(target == 0 &&
     (mode & TRACK_OBJECT_MODE_DIRECTION_BIT) == 0)
    {
      target = find_track_object_in_direction(
        (TrackInteractionObject *)(void *)object,
        track_effect_type_masks[mode], mode,
        TRACK_OBJECT_TARGET_SEARCH_BASE +
        double_width * TRACK_OBJECT_SEARCH_SCALE);
      object->target = target;
    }

  lane = object->lane;
  candidate = find_track_object_in_lane(
    (TrackInteractionObject *)(void *)object, lane, mode,
    TRACK_OBJECT_NEARBY_TYPE_MASK,
    double_width * TRACK_OBJECT_SEARCH_SCALE);
  object->nearby_object = candidate;
  if(candidate != 0)
    {
      distance = measure_track_object_distance(&object->position, &candidate->position,
                                               mode);
      if(distance <
         divide_by_two_toward_zero(width + double_width) &&
         (candidate->collision_flags &
          TRACK_OBJECT_BLOCKING_COLLISION_FLAG) == 0)
        {
          if(mode == TRACK_OBJECT_MODE_FORWARD &&
             candidate->world_velocity.forward <
             object->world_velocity.forward)
            {
              if(lane != 0)
                {
                  lane--;
                  object->lane = (signed char)lane;
                  lane_changed = 1;
                  goto lane_check;
                }

              road = object->primary_track;
              if(divide_by_position_unit_toward_zero(
                   road->right_width) > 1)
                {
                  object->lane = 1;
                  lane_changed = 1;
                  goto lane_check;
                }
            }
          else if(mode == TRACK_OBJECT_MODE_REVERSE &&
                  (candidate->collision_flags &
                   TRACK_OBJECT_CROSSING_COLLISION_FLAGS) == 0)
            {
              if(lane < -1)
                {
                  lane++;
                  object->lane = (signed char)lane;
                  lane_changed = 1;
                  goto lane_check;
                }

              road = object->primary_track;
              if(divide_by_position_unit_toward_zero(
                   road->left_width) > -lane)
                {
                  lane--;
                  object->lane = (signed char)lane;
                  lane_changed = 1;
                  goto lane_check;
                }
            }
        }

      if(distance < double_width)
        {
          if(mode == TRACK_OBJECT_MODE_FORWARD)
            {
              coordinate = candidate->world_velocity.forward;
            }
          else if(mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
            {
              coordinate = candidate->world_velocity.lateral;
            }
          else if(mode == TRACK_OBJECT_MODE_REVERSE)
            {
              coordinate = -candidate->world_velocity.forward;
            }
          else
            {
              coordinate = -candidate->world_velocity.lateral;
            }

          speed = approach_track_object_speed(
            &object->position, road_speed, base_speed, 0,
            &candidate->position, scale_track_velocity(coordinate),
            TRACK_OBJECT_SPEED_ADJUSTMENT, double_width, -width, mode);
          collision_adjusted = 1;
        }
    }

lane_check:
  mode = object->mode;
  lane = object->lane;
  if((mode & TRACK_OBJECT_MODE_DIRECTION_BIT) == 0)
    {
      road = object->primary_track;
      if(!track_lane_is_valid(road, mode, lane))
        {
          int delta;
          int test_lane;

          road = select_adjacent_road_segment(road, mode);
          if(!track_lane_is_valid(road, mode, lane))
            {
              delta = mode == TRACK_OBJECT_MODE_FORWARD ? -1 : 1;
              test_lane = lane + delta;
              if(track_lane_is_valid(road, mode, test_lane))
                {
                  lane += delta;
                  object->lane = (signed char)lane;
                }

              test_lane = lane - delta;
              if(track_lane_is_valid(road, mode, test_lane))
                {
                  lane -= delta;
                  object->lane = (signed char)lane;
                }
              else
                {
                  speed = 0;
                  distance = 0;
                  collision_adjusted = 1;
                }
            }
        }
    }

  target = object->target;
  if(target != 0 && speed > 0)
    {
      mode = object->mode;
      if(mode == TRACK_OBJECT_MODE_FORWARD)
        {
          limit = target->motion_source->world_velocity.forward +
                  TRACK_OBJECT_TARGET_LONGITUDINAL_MARGIN;
          target_delta = target->position.track_position - limit +
                         object->collision_half_length;
          target_offset =
            (limit - (object->position.track_position +
                      object->collision_half_length)) >>
            TRACK_OBJECT_DISTANCE_FRACTION_BITS;
        }
      else if(mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
        {
          limit = object->track_segment->road_left -
                  TRACK_OBJECT_TARGET_LATERAL_MARGIN;
          target_delta = divide_by_position_unit_toward_zero(
            target->position.lateral_position) -
                         limit +
                         (object->collision_half_width >>
                          TRACK_OBJECT_POSITION_FRACTION_BITS);
          target_offset =
            ((limit << TRACK_OBJECT_POSITION_FRACTION_BITS) -
             (object->position.lateral_position +
              object->collision_half_width)) >>
            TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
        }
      else if(mode == TRACK_OBJECT_MODE_REVERSE)
        {
          limit = target->motion_source->world_velocity.lateral -
                  TRACK_OBJECT_TARGET_LONGITUDINAL_MARGIN;
          target_delta = limit - target->position.track_position +
                         object->collision_half_length;
          target_offset =
            (object->position.track_position -
             object->collision_half_length - limit) >>
            TRACK_OBJECT_DISTANCE_FRACTION_BITS;
        }
      else
        {
          limit = object->track_segment->road_right +
                  TRACK_OBJECT_TARGET_LATERAL_MARGIN;
          target_delta =
            limit - divide_by_position_unit_toward_zero(
              target->position.lateral_position) +
            (object->collision_half_width >>
             TRACK_OBJECT_POSITION_FRACTION_BITS);
          target_offset =
            (object->position.lateral_position -
             object->collision_half_width -
             (limit << TRACK_OBJECT_POSITION_FRACTION_BITS)) >>
            TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
        }

      if((mode & TRACK_OBJECT_MODE_DIRECTION_BIT) == 0)
        {
          projected = object->time_step *
                      (speed * TRACK_OBJECT_PREDICTION_SCALE);
          required = target_offset <<
                     TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
          if(projected > required)
            {
              road = target->track_segment;
              if((mode == TRACK_OBJECT_MODE_FORWARD &&
                  divide_by_position_unit_toward_zero(
                    road->right_width) < 1) ||
                 (mode != TRACK_OBJECT_MODE_FORWARD &&
                  divide_by_position_unit_toward_zero(
                    road->left_width) < 1))
                {
                  speed = 0;
                  goto output_control;
                }
            }
        }

      if(speed > 0 && target_offset <= distance)
        {
          if(target_offset < -TRACK_OBJECT_TARGET_PASSED_MARGIN &&
             -(target_delta >> TRACK_OBJECT_DISTANCE_FRACTION_BITS) >
             target_offset)
            {
              object->target = 0;
              goto output_control;
            }

          if(target->interaction_selector ==
             TRACK_OBJECT_MODE_CROSS_POSITIVE)
            {
              desired_offset = road_margin;
            }
          else if(target->interaction_selector ==
                  TRACK_OBJECT_MODE_REVERSE)
            {
              desired_offset = -TRACK_OBJECT_TARGET_PASSED_MARGIN;
            }
          else
            {
              goto output_control;
            }

          limit = double_width;
          if(limit < TRACK_OBJECT_MINIMUM_APPROACH_LIMIT)
            {
              limit = TRACK_OBJECT_MINIMUM_APPROACH_LIMIT;
            }
          if(target_offset <= desired_offset)
            {
              if(!collision_adjusted)
                {
                  boost = 1;
                }
              goto output_control;
            }

          if(target_offset < limit && target_offset > 0)
            {
              speed = approach_track_object_speed(
                &object->position, road_speed, base_speed, 0,
                &target->position, 0, TRACK_OBJECT_SPEED_ADJUSTMENT,
                limit,
                -(target_delta >> TRACK_OBJECT_DISTANCE_FRACTION_BITS),
                mode);
              projected = object->time_step *
                          (speed * TRACK_OBJECT_APPROACH_SCALE);
              required = target_offset <<
                         TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS;
              while(projected > required)
                {
                  speed = divide_by_two_toward_zero(speed);
                  projected = object->time_step *
                              (speed * TRACK_OBJECT_APPROACH_SCALE);
                }
            }
        }
    }

output_control:
  mode = object->mode;
  object->motion_command.direction =
    track_direction_modes[TRACK_OBJECT_DIRECTION_TABLE_BASE + mode] *
    TRACK_OBJECT_DIRECTION_SCALE;

  lane = object->lane;
  road = object->track_segment;
  desired_offset =
    (road->center_position + lane * TRACK_OBJECT_LANE_SPACING) *
    TRACK_OBJECT_POSITION_UNIT +
    TRACK_OBJECT_LANE_CENTER_BIAS - object->position.lateral_position;
  if(lane_changed)
    {
      object->motion_command.lateral_offset += divide_by_two_toward_zero(
        desired_offset - object->motion_command.lateral_offset);
    }
  else
    {
      object->motion_command.lateral_offset = desired_offset;
    }

  if(mode == TRACK_OBJECT_MODE_CROSS_POSITIVE)
    {
      object->motion_command.lateral_offset =
        TRACK_OBJECT_CROSS_TRACK_OFFSET;
      object->motion_command.direction = 0;
    }
  else if(mode == TRACK_OBJECT_MODE_CROSS_NEGATIVE)
    {
      object->motion_command.lateral_offset =
        -TRACK_OBJECT_CROSS_TRACK_OFFSET;
      object->motion_command.direction = 0;
    }

  if(object->delay_ticks > 0)
    {
      speed = 0;
    }
  else if(speed < 0)
    {
      speed = 0;
    }
  if(boost)
    {
      speed *= TRACK_OBJECT_WIDTH_MULTIPLIER;
    }

  acceleration_scale = TRACK_OBJECT_NORMAL_ACCELERATION_SCALE;
  if(divide_by_two_toward_zero(speed) > road_speed || boost)
    {
      acceleration_scale = TRACK_OBJECT_FAST_ACCELERATION_SCALE;
    }
  apply_track_object_steering(
    object, &object->motion_command,
    speed * TRACK_OBJECT_APPROACH_SCALE,
    (object->cruise_speed * acceleration_scale) >>
    TRACK_OBJECT_POSITION_FRACTION_BITS);

  animation = &object->animation;
  distance = absolute_int_difference(
    object->position.track_position,
    track_simulation_globals.motion_source.packed_track_position);
  if(distance > TRACK_OBJECT_ANIMATION_FAR_DISTANCE &&
     animation_state_is_available(animation,
                                  TRACK_OBJECT_ANIMATION_FAR))
    {
      set_primary_rider_animation(animation, TRACK_OBJECT_ANIMATION_FAR);
    }
  else if(distance > TRACK_OBJECT_ANIMATION_NEAR_DISTANCE &&
          animation_state_is_available(animation,
                                       TRACK_OBJECT_ANIMATION_NEAR))
    {
      set_primary_rider_animation(animation, TRACK_OBJECT_ANIMATION_NEAR);
    }
  else
    {
      set_primary_rider_animation(animation, TRACK_OBJECT_MODE_FORWARD);
    }
  advance_car_animation(animation);
}


void
track_object_mode_update(RacerEntity *racer)
{
  TrackObject *object;

  object = (TrackObject *)racer;
  object->mode_update_seen = 1;
  if((object->mode & TRACK_OBJECT_MODE_DIRECTION_BIT) == 0 ||
     object->delay_ticks != 0)
    {
      return;
    }

  object->delay_ticks = TRACK_OBJECT_SHORT_DELAY_TICKS;
  if((object->contact->collision_flags &
      TRACK_OBJECT_CROSSING_COLLISION_FLAGS) != 0)
    {
      object->delay_ticks = TRACK_OBJECT_LONG_DELAY_TICKS;
    }
}


int
track_object_draw(TrackObject *object,
                  int          bitmap)
{
  return car_animation_render(
    (CansAnimationRenderObject *)(void *)object,
    (RoadProjectedRenderContext *)bitmap);
}
