#include "stdlib.h"

#include "division_helpers.h"
#include "race_object_lifecycle.h"
#include "road_projected_cel.h"
#include "track_effect_runtime.h"

enum TrackEffectConstant
{
  TRACK_EFFECT_SIGN_SHIFT = 31,
  TRACK_EFFECT_FIXED_SHIFT = 8,
  TRACK_EFFECT_FIXED_ONE = 0x100,
  TRACK_EFFECT_PACKED_EXTENT_MASK = 0x1FF,
  TRACK_EFFECT_PACKED_SHIFT_MASK = 0x1F,
  TRACK_EFFECT_INITIAL_TIMER = 0x30C,
  TRACK_EFFECT_INITIAL_EVEN_PHASE = 2,
  TRACK_EFFECT_TYPE_FLIP_MASK = 2,
  TRACK_EFFECT_LEFT_BOUND_MARGIN = 0x100,
  TRACK_EFFECT_RIGHT_BOUND_MARGIN = 0x200,
  TRACK_EFFECT_PHASE_FRAME_BIT = 8,
  TRACK_EFFECT_LAST_PHASE = 2,
  TRACK_EFFECT_SPAWN_USE_CHILD_TYPE = 1,
  TRACK_EFFECT_SPAWN_FORCE_TYPE_THREE = 2,
  TRACK_EFFECT_SPAWN_TYPE_ONE = 1,
  TRACK_EFFECT_SPAWN_TYPE_THREE = 3,
  TRACK_EFFECT_SPAWN_DEPTH = 0x78000,
  TRACK_EFFECT_RENDER_BASE_SCALE = 0x200,
  TRACK_EFFECT_RENDER_MAPPING_MODE = 1,
  TRACK_EFFECT_PLUT_CLEAR_BYTE_A = 0x3A,
  TRACK_EFFECT_PLUT_CLEAR_BYTE_B = 0x3B
};

void
create_track_object(int                     event_type,
                    int                     lateral_offset,
                    int                     depth_offset,
                    TrackEffectObject      *source,
                    RoadSegmentLaneRuntime *target_segment);

static
int
divide_by_two_toward_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> TRACK_EFFECT_SIGN_SHIFT)) >> 1;
}


static
int
fixed_8_8_to_integer_toward_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_EFFECT_FIXED_ONE - 1;
    }
  return value >> TRACK_EFFECT_FIXED_SHIFT;
}


static
int
sign_extend_byte(int value)
{
  return (signed char)value;
}


static
int
decode_packed_cel_extent(int value)
{
  if(value >= 0)
    {
      return value & TRACK_EFFECT_PACKED_EXTENT_MASK;
    }
  return 1 << (value & TRACK_EFFECT_PACKED_SHIFT_MASK);
}


static
int
track_effect_side_center(const RoadSegmentLaneRuntime *segment,
                         int                           side)
{
  int road_edge;
  int shoulder_edge;

  if(side == TRACK_EFFECT_RIGHT_SIDE)
    {
      road_edge = segment->road_right;
      shoulder_edge = segment->shoulder_right;
    }
  else
    {
      road_edge = segment->road_left;
      shoulder_edge = segment->shoulder_left;
    }
  return divide_by_two_toward_zero(road_edge + shoulder_edge) <<
         TRACK_EFFECT_FIXED_SHIFT;
}


TrackEffectObject *
create_track_effect_child(int                     effect_type,
                          int                     side,
                          int                     interpolation_fraction,
                          RoadSegmentLaneRuntime *segment,
                          int                     spread_offset,
                          TrackEffectObject      *parent)
{
  RoadSegmentLaneRuntime *next_segment;
  TrackEffectObject *effect;
  int base_position;
  int next_position;
  int interpolated_delta;
  int lateral_position;

  base_position = track_effect_side_center(segment, side);
  next_segment = segment->previous_links[side];
  next_position = track_effect_side_center(next_segment, side);
  interpolated_delta =
    interpolation_fraction * (next_position - base_position);
  lateral_position = base_position +
                     fixed_8_8_to_integer_toward_zero(interpolated_delta);

  track_effect_child_descriptor.object_type =
    track_effect_type_masks[effect_type];
  effect = (TrackEffectObject *)create_race_object_from_descriptor(
    (IntrusiveListHeader *)track_effect_pools.child_pool,
    (const RaceObjectCreationDescriptor *)
    &track_effect_child_descriptor,
    lateral_position, interpolation_fraction, segment);
  if(effect == 0)
    {
      return 0;
    }

  effect->state.child.parent = parent;
  if((effect_type & 1) == 0)
    {
      effect->state.child.phase_index = TRACK_EFFECT_INITIAL_EVEN_PHASE;
    }
  else
    {
      effect->state.child.phase_index = 0;
    }
  effect->state.child.phase_timer = TRACK_EFFECT_INITIAL_TIMER;
  effect->state.child.effect_type = effect_type;
  effect->state.child.spread_offset = (signed char)spread_offset;
  return effect;
}


void
track_effect_parent_update(TrackEffectObject *effect)
{
  TrackEffectObject *child;
  RoadSegmentLaneRuntime *target_segment;
  int child_index;
  int phase_index;
  int phase_duration;
  int event_type;
  int random_spread;
  int phase_offset;
  int random_offset;
  int target_position;
  int depth_offset;
  SignedDivisionResult random_roll;

  child_index = 0;
  while(child_index < TRACK_EFFECT_CHILD_COUNT)
    {
      child = effect->state.parent.children[child_index];
      if(child != 0)
        {
          child->state.child.phase_timer += effect->time_step;

          if(track_runtime_globals.left_bound +
             TRACK_EFFECT_LEFT_BOUND_MARGIN <=
             child->track_position &&
             track_runtime_globals.right_bound -
             TRACK_EFFECT_RIGHT_BOUND_MARGIN >=
             child->track_position)
            {
              phase_index = child->state.child.phase_index;
              phase_duration =
                effect->state.parent.phase_durations[phase_index];
              while(phase_duration < child->state.child.phase_timer)
                {
                  child->state.child.phase_timer -= phase_duration;
                  phase_index = sign_extend_byte(phase_index + 1);
                  if(phase_index > TRACK_EFFECT_LAST_PHASE)
                    {
                      phase_index = 0;
                    }
                  child->state.child.phase_index = phase_index;

                  if((child->state.child.effect_type & 1) != 0 &&
                     phase_index == 0 &&
                     (frame_tick & TRACK_EFFECT_PHASE_FRAME_BIT) != 0)
                    {
                      phase_offset = 0;
                      if(effect->state.parent.spawn_mode ==
                         TRACK_EFFECT_SPAWN_USE_CHILD_TYPE)
                        {
                          event_type = child->state.child.effect_type;
                          random_spread = divide_by_two_toward_zero(
                            effect->state.parent.spawn_spread);
                          random_spread = sign_extend_byte(random_spread);
                          if(event_type ==
                             TRACK_EFFECT_SPAWN_TYPE_THREE)
                            {
                              phase_offset = random_spread;
                            }
                        }
                      else
                        {
                          if(effect->state.parent.spawn_mode ==
                             TRACK_EFFECT_SPAWN_FORCE_TYPE_THREE)
                            {
                              event_type =
                                TRACK_EFFECT_SPAWN_TYPE_THREE;
                            }
                          else
                            {
                              event_type = TRACK_EFFECT_SPAWN_TYPE_ONE;
                            }
                          random_spread = sign_extend_byte(
                            effect->state.parent.spawn_spread);
                        }

                      random_roll = signed_divide_with_remainder(
                        (unsigned int)random_spread,
                        (unsigned int)rand());
                      random_offset = sign_extend_byte(
                        random_roll.remainder + phase_offset);
                      target_position =
                        effect->state.parent.spawn_track_position +
                        random_offset * TRACK_EFFECT_FIXED_ONE +
                        TRACK_EFFECT_FIXED_ONE;
                      target_segment =
                        find_road_lane_at_track_position(
                          target_position, child->track_segment);
                      if(event_type ==
                         TRACK_EFFECT_SPAWN_TYPE_THREE)
                        {
                          depth_offset = TRACK_EFFECT_SPAWN_DEPTH;
                        }
                      else
                        {
                          depth_offset = -TRACK_EFFECT_SPAWN_DEPTH;
                        }
                      create_track_object(
                        event_type, random_offset, depth_offset,
                        child, target_segment);
                    }

                  phase_duration =
                    effect->state.parent.phase_durations[phase_index];
                }
            }
        }
      child_index = sign_extend_byte(child_index + 1);
    }
}


void
track_effect_parent_destroy(TrackEffectObject *effect)
{
  TrackEffectObject *child;
  int child_index;

  child_index = 0;
  while(child_index < TRACK_EFFECT_CHILD_COUNT)
    {
      child = effect->state.parent.children[child_index];
      if(child != 0)
        {
          schedule_race_object_removal(child);
        }
      child_index = sign_extend_byte(child_index + 1);
    }
  schedule_race_object_removal(effect);
}


void
track_effect_child_destroy(void)
{
}


void
track_effect_child_draw(TrackEffectObject          *effect,
                        RoadProjectedRenderContext *render_context)
{
  TrackEffectObject *parent;
  TrackEffectObject *phase_source_effect;
  CCB *source;
  unsigned char *palette;
  int x;
  int frame;

  source = track_effect_pools.cel_source;
  if(source == 0)
    {
      return;
    }

  palette = (unsigned char *)source->ccb_PLUTPtr;
  palette[TRACK_EFFECT_PLUT_CLEAR_BYTE_A] = 0;
  palette[TRACK_EFFECT_PLUT_CLEAR_BYTE_B] = 0;

  effect->render_descriptor.source_ccb = source;
  effect->render_descriptor.horizontal_extent =
    divide_by_two_toward_zero(
      decode_packed_cel_extent(source->ccb_Width));
  effect->render_descriptor.vertical_extent =
    decode_packed_cel_extent(source->ccb_Height);
  effect->render_descriptor.flags =
    (effect->state.child.effect_type & TRACK_EFFECT_TYPE_FLIP_MASK) == 0;

  x = effect->render_x +
      (effect->render_x_fraction_8_8 >> TRACK_EFFECT_FIXED_SHIFT);
  frame = fixed_8_8_to_integer_toward_zero(
    effect->render_frame_8_8);
  render_road_projected_cel(
    render_context, &effect->render_descriptor,
    TRACK_EFFECT_RENDER_BASE_SCALE, frame, x, effect->render_y,
    TRACK_EFFECT_RENDER_MAPPING_MODE);

  parent = effect->state.child.parent;
  phase_source_effect = parent->state.parent.children[0];
  if(phase_source_effect == 0)
    {
      return;
    }

  source = track_effect_resources[
    phase_source_effect->state.child.phase_index];
  effect->render_descriptor.source_ccb = source;
  if(source == 0)
    {
      return;
    }

  render_road_projected_cel(
    render_context, &effect->render_descriptor,
    TRACK_EFFECT_RENDER_BASE_SCALE, frame, x, effect->render_y,
    TRACK_EFFECT_RENDER_MAPPING_MODE);
}
