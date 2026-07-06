#include "intrusive_list.h"
#include "track_effect_runtime.h"

enum TrackEffectParentConstant
{
  TRACK_EFFECT_MODE_COUNT = 4,
  TRACK_EFFECT_SPECIAL_RESOURCE_KIND = 2,
  TRACK_EFFECT_SPECIAL_GEOMETRY_MODE = 4,
  TRACK_EFFECT_MODE_TWO = 2,
  TRACK_EFFECT_MODE_THREE = 3,
  TRACK_EFFECT_CHILD_OUTER_LEFT = 2,
  TRACK_EFFECT_CHILD_INNER_LEFT = 3,
  TRACK_EFFECT_THIRD_PHASE = 2,
  TRACK_EFFECT_CELL_SHIFT = 8,
  TRACK_EFFECT_CELL_SIZE = 0x100,
  TRACK_EFFECT_SPAN_MARGIN = 2,
  TRACK_EFFECT_EARLY_INTERPOLATION = 0x50,
  TRACK_EFFECT_LATE_INTERPOLATION = 0xB0,
  TRACK_EFFECT_SHORT_FIRST_PHASE = 0x4B,
  TRACK_EFFECT_SHORT_SECOND_PHASE = 0,
  TRACK_EFFECT_SHORT_THIRD_PHASE = 0xF0,
  TRACK_EFFECT_NORMAL_FIRST_PHASE = 0x258,
  TRACK_EFFECT_NORMAL_SECOND_PHASE = 0xB4,
  TRACK_EFFECT_NORMAL_THIRD_PHASE = 0x30C
};

typedef struct TrackEffectRoadResource
{
  unsigned char header[8];
  int track_position;
  unsigned char retained_00c[0x10];
  unsigned char resource_kind;
} TrackEffectRoadResource;

static
int
fixed_8_8_to_integer_towards_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_EFFECT_CELL_SIZE - 1;
    }
  return value >> TRACK_EFFECT_CELL_SHIFT;
}


static
int
uses_short_track_effect_timing(const TrackEffectRoadResource *resource,
                               const RoadSegmentLaneRuntime  *segment)
{
  return resource->resource_kind == TRACK_EFFECT_SPECIAL_RESOURCE_KIND &&
         segment->geometry_mode == TRACK_EFFECT_SPECIAL_GEOMETRY_MODE;
}


static
void
clear_track_effect_child_render_callbacks(TrackEffectObject *parent)
{
  TrackEffectObject *child;
  int child_index;

  child_index = 0;
  do
    {
      child = parent->state.parent.children[child_index];
      if(child != 0)
        {
          child->render = 0;
        }
      child_index++;
    } while(child_index < TRACK_EFFECT_CHILD_COUNT);
}


void
spawn_track_effect_parent(const TrackEffectSpawnRecord *record,
                          RoadSegmentLaneRuntime       *origin_segment)
{
  TrackEffectRoadResource *resource;
  RoadSegmentLaneRuntime *start_segment;
  RoadSegmentLaneRuntime *end_segment;
  RoadSegmentLaneRuntime *spawn_segment;
  TrackEffectObject *parent;
  TrackEffectObject *child;
  int effect_mode;
  int clear_child_callbacks;
  int short_timing;
  int cell_origin;
  int segment_span;
  int road_width;
  int spawn_position;
  int child_index;

  effect_mode = (signed char)(record->encoded_mode + 1);
  if(effect_mode >= TRACK_EFFECT_MODE_COUNT)
    {
      return;
    }

  clear_child_callbacks = record->preserve_child_updates == 0;
  resource = (TrackEffectRoadResource *)origin_segment->resource_handle;
  short_timing = uses_short_track_effect_timing(resource, origin_segment);

  cell_origin =
    (resource->track_position & ~(TRACK_EFFECT_CELL_SIZE - 1)) -
    (origin_segment->clip_position & ~(TRACK_EFFECT_CELL_SIZE - 1));
  start_segment = find_road_lane_at_track_position(
    cell_origin + record->start_cell * TRACK_EFFECT_CELL_SIZE,
    origin_segment);
  end_segment = find_road_lane_at_track_position(
    cell_origin + record->end_cell * TRACK_EFFECT_CELL_SIZE -
    TRACK_EFFECT_CELL_SIZE,
    start_segment);

  segment_span = (signed char)(record->end_cell - record->start_cell -
                               TRACK_EFFECT_SPAN_MARGIN);
  road_width = (signed char)(
    fixed_8_8_to_integer_towards_zero(start_segment->right_width) +
    fixed_8_8_to_integer_towards_zero(start_segment->left_width));

  spawn_position = resource->track_position;
  if(spawn_position > track_runtime_globals.right_bound)
    {
      spawn_position = track_runtime_globals.right_bound;
    }
  else if(spawn_position < track_runtime_globals.left_bound)
    {
      spawn_position = track_runtime_globals.left_bound;
    }

  spawn_segment = find_road_lane_at_track_position(
    spawn_position, origin_segment);
  parent = (TrackEffectObject *)create_race_object_from_descriptor(
    (IntrusiveListHeader *)track_effect_pools.parent_pool,
    (const RaceObjectCreationDescriptor *)
    &track_effect_parent_descriptor,
    spawn_segment->center_position, 0, spawn_segment);
  if(parent == 0)
    {
      return;
    }

  for(child_index = 0; child_index < TRACK_EFFECT_CHILD_COUNT; child_index++)
    {
      parent->state.parent.children[child_index] = 0;
    }

  parent->state.parent.spawn_track_position = record->start_cell;
  parent->state.parent.spawn_end_position = record->end_cell;
  parent->state.parent.spawn_mode = (unsigned char)effect_mode;
  parent->state.parent.spawn_spread = record->spread;
  parent->state.parent.simulation_scale = record->simulation_scale;
  parent->state.parent.preserve_child_updates =
    record->preserve_child_updates;
  parent->state.parent.spawn_track_position =
    cell_origin + parent->state.parent.spawn_track_position *
    TRACK_EFFECT_CELL_SIZE;
  parent->state.parent.spawn_end_position =
    cell_origin + parent->state.parent.spawn_end_position *
    TRACK_EFFECT_CELL_SIZE;

  if(short_timing && clear_child_callbacks)
    {
      parent->state.parent.phase_durations[0] =
        TRACK_EFFECT_SHORT_FIRST_PHASE;
      parent->state.parent.phase_durations[1] =
        TRACK_EFFECT_SHORT_SECOND_PHASE;
      parent->state.parent.phase_durations[TRACK_EFFECT_THIRD_PHASE] =
        TRACK_EFFECT_SHORT_THIRD_PHASE;
    }
  else
    {
      parent->state.parent.phase_durations[0] =
        TRACK_EFFECT_NORMAL_FIRST_PHASE;
      parent->state.parent.phase_durations[1] =
        TRACK_EFFECT_NORMAL_SECOND_PHASE;
      parent->state.parent.phase_durations[TRACK_EFFECT_THIRD_PHASE] =
        TRACK_EFFECT_NORMAL_THIRD_PHASE;
    }

  if(fixed_8_8_to_integer_towards_zero(start_segment->right_width) > 0)
    {
      child = create_track_effect_child(
        0, TRACK_EFFECT_RIGHT_SIDE, TRACK_EFFECT_EARLY_INTERPOLATION,
        end_segment, segment_span, parent);
      parent->state.parent.children[0] = child;
      if(child == 0)
        {
          goto fail;
        }
    }

  if(fixed_8_8_to_integer_towards_zero(end_segment->left_width) > 0)
    {
      child = create_track_effect_child(
        TRACK_EFFECT_CHILD_OUTER_LEFT, TRACK_EFFECT_LEFT_SIDE,
        TRACK_EFFECT_LATE_INTERPOLATION,
        start_segment, segment_span, parent);
      parent->state.parent.children[TRACK_EFFECT_CHILD_OUTER_LEFT] = child;
      if(child == 0)
        {
          goto fail;
        }
    }

  if(!short_timing)
    {
      if(effect_mode == 1 || effect_mode == TRACK_EFFECT_MODE_TWO)
        {
          child = create_track_effect_child(
            TRACK_EFFECT_CHILD_INNER_LEFT, TRACK_EFFECT_LEFT_SIDE,
            TRACK_EFFECT_EARLY_INTERPOLATION,
            end_segment, road_width, parent);
          parent->state.parent.children[TRACK_EFFECT_CHILD_INNER_LEFT] =
            child;
          if(child == 0)
            {
              goto fail;
            }
        }
      if(effect_mode == 1 || effect_mode == TRACK_EFFECT_MODE_THREE)
        {
          child = create_track_effect_child(
            1, TRACK_EFFECT_RIGHT_SIDE, TRACK_EFFECT_LATE_INTERPOLATION,
            start_segment, road_width, parent);
          parent->state.parent.children[1] = child;
          if(child == 0)
            {
              goto fail;
            }
        }
    }

  if(clear_child_callbacks)
    {
      clear_track_effect_child_render_callbacks(parent);
    }
  return;

fail:
  for(child_index = 0; child_index < TRACK_EFFECT_CHILD_COUNT; child_index++)
    {
      child = parent->state.parent.children[child_index];
      if(child != 0)
        {
          return_pooled_node_to_owner((PooledListNode *)child);
          parent->state.parent.children[child_index] = 0;
        }
    }
  return_pooled_node_to_owner((PooledListNode *)parent);
}
