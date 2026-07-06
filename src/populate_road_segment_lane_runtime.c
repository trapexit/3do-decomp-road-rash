#include "race_object_creation.h"
#include "road_geometry.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"
#include "track_segment_definition.h"

/* Reconstructed from the original function at 0x00023428. */

int
fixed_arctangent_ratio_8_8(int ratio);
void
emit_track_hazards_for_segment(TrackHazardTraversal *traversal,
                               RoadSegmentLaneRuntime *runtime_lane);
typedef struct TrackEffectSpawnRecord TrackEffectSpawnRecord;
void
apply_track_effect_spawn_record(TrackEffectSpawnRecord *record,
                                RoadSegmentLaneRuntime *origin_segment);

#define ROAD_LANE_RESOURCE_POSITION_OFFSET 0x08
#define ROAD_LANE_RESOURCE_VARIANT_OFFSET 0x04
#define ROAD_LANE_RESOURCE_TYPE_OFFSET 0x1C
#define TRACK_POSITION_ROUND_BIAS 0xFF
#define TRACK_POSITION_ONE (1 << TRACK_POSITION_FRACTION_BITS)
#define ROAD_WIDTH_TRACK_SCALE 0xFA
#define ROAD_PROJECTION_SHIFT 14
#define INTEGER_SIGN_BIT_SHIFT 31
#define ROAD_OBJECT_SAMPLE_MODE 0x80
#define ROAD_COLLISION_KIND_LEVEL_GATE 7
#define ROAD_TOPOLOGY_LEVEL_MATCH_FLAG 0x20
#define ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG 0x40
#define ROAD_BOUND_REFLECTION_SCALE 2
#define ROAD_EDGE_MIN_EXTENSION 0x320
#define ROAD_EDGE_DEFAULT_DEPTH 0x10
#define ROAD_RESOURCE_TYPE_DUAL 2
#define ROAD_SURFACE_ENTRY_FLAG 8
#define ROAD_SURFACE_TERMINAL_FLAG ((int)0x80000000)
#define ROAD_EDGE_LEFT 0
#define ROAD_EDGE_RIGHT 1

static
int
road_clip_finish_distance(const RoadClipPlaybackState *playback)
{
  TrackSegmentDefinition *definition;

  definition = playback->active_clip->payload.segment_definition;
  return definition->finish_sample_offset;
}


static
int
lane_resource_track_position(const RoadSegmentLaneRuntime *lane_runtime)
{
  char *resource;

  resource = (char *)lane_runtime->resource_handle;
  return *(int *)(resource + ROAD_LANE_RESOURCE_POSITION_OFFSET);
}


static
int
lane_resource_is_dual(const RoadSegmentLaneRuntime *lane_runtime)
{
  char *resource;
  char *variant;

  resource = (char *)lane_runtime->resource_handle;
  variant = *(char **)(resource + ROAD_LANE_RESOURCE_VARIANT_OFFSET);
  return *(unsigned char *)(variant + ROAD_LANE_RESOURCE_TYPE_OFFSET) ==
         ROAD_RESOURCE_TYPE_DUAL;
}


static
unsigned char
lane_object_position_count(const RoadSegmentLaneRuntime *lane_runtime)
{
  return lane_runtime->repeated_objects.placement_count;
}


static
int
lane_object_position(const RoadSegmentLaneRuntime *lane_runtime,
                     int                           object_index)
{
  return lane_runtime->repeated_objects
         .lateral_positions[object_index];
}


static
TrackEffectSpawnRecord *
surface_activation_record(const RoadClipPlaybackState *playback)
{
  return (TrackEffectSpawnRecord *)playback->section_state.active_entry;
}


static
TrackHazardTraversal *
clip_hazard_traversal(RoadClipPlaybackState *playback)
{
  return &playback->hazard_traversal;
}


static
int
divide_fixed_byte_towards_zero(int value)
{
  if(value < 0)
    {
      value += TRACK_POSITION_ROUND_BIAS;
    }
  return value >> TRACK_POSITION_FRACTION_BITS;
}


static
int
halve_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> INTEGER_SIGN_BIT_SHIFT)) >> 1;
}


static
int
scale_road_width_to_track_units(int value)
{
  value *= ROAD_WIDTH_TRACK_SCALE;
  if(value < 0)
    {
      value += TRACK_POSITION_ROUND_BIAS;
    }
  return value >> TRACK_POSITION_FRACTION_BITS;
}


void
populate_road_segment_lane_runtime(RoadSegmentLaneRuntime *lane_runtime,
                                   RoadClipPlaybackState  *playback,
                                   unsigned int            direction)
{
  RoadObjectTraversalState *object_state;
  int traversal_direction;
  int finish_distance;
  int selected_level;
  int left_width;
  int right_width;
  int rounded_left_width;
  int rounded_right_width;
  int left_rounding_input;
  int right_rounding_input;
  int center_position;
  int left_bound;
  int right_bound;
  int surface_flags;
  int geometry_mode;
  int profile_index;
  int object_index;
  int object_position;
  int road_right;
  int road_left;

  reset_roadside_collision_binding_state(lane_runtime);

  traversal_direction = direction & TRACK_DIRECTION_BYTE_MASK;
  if(traversal_direction != 0 &&
     track_runtime_globals.finish_threshold == 0)
    {
      finish_distance = road_clip_finish_distance(playback);
      if(finish_distance != 0)
        {
          track_runtime_globals.finish_threshold =
            lane_resource_track_position(lane_runtime) +
            finish_distance * TRACK_POSITION_ONE;
        }
    }

  lane_runtime->clip_node = playback->active_clip;
  lane_runtime->clip_position =
    playback->clip_position & ~TRACK_DIRECTION_BYTE_MASK;
  lane_runtime->center_position = playback->center_position;

  sample_road_path_steps(
    &playback->path_state,
    &lane_runtime->path_curvature_step,
    &lane_runtime->path_elevation_step);
  object_state = &playback->object_state;
  populate_repeated_roadside_object_group(
    object_state, lane_runtime);
  road_family_schedule_lane_hook(
    &playback->family_schedule_state);

  lane_runtime->curvature_projection_step =
    fixed_arctangent_ratio_8_8(lane_runtime->path_curvature_step) <<
      ROAD_PROJECTION_SHIFT;
  lane_runtime->elevation_projection_step =
    fixed_arctangent_ratio_8_8(lane_runtime->path_elevation_step) <<
      ROAD_PROJECTION_SHIFT;
  lane_runtime->path_elevation =
    get_road_path_elevation(&playback->path_state);

  sample_road_lane_widths(&playback->lane_width_state,
                          &lane_runtime->left_width,
                          &lane_runtime->right_width);

  left_width = lane_runtime->left_width;
  left_rounding_input = left_width + TRACK_POSITION_ROUND_BIAS;
  rounded_left_width =
    divide_fixed_byte_towards_zero(left_rounding_input);
  center_position = lane_runtime->center_position;
  left_bound = center_position -
               rounded_left_width * ROAD_WIDTH_TRACK_SCALE;
  lane_runtime->segment_left_bound = left_bound;

  right_width = lane_runtime->right_width;
  right_rounding_input = right_width + TRACK_POSITION_ROUND_BIAS;
  rounded_right_width =
    divide_fixed_byte_towards_zero(right_rounding_input);
  right_bound = center_position +
                rounded_right_width * ROAD_WIDTH_TRACK_SCALE;
  lane_runtime->connected_left_bound = left_bound;
  lane_runtime->segment_right_bound = right_bound;
  lane_runtime->connected_right_bound = right_bound;
  lane_runtime->left_width_cells =
    (unsigned char)(left_rounding_input >> TRACK_POSITION_FRACTION_BITS);
  lane_runtime->right_width_cells =
    (unsigned char)(right_rounding_input >> TRACK_POSITION_FRACTION_BITS);

  left_bound = center_position -
               scale_road_width_to_track_units(left_width);
  right_bound = center_position +
                scale_road_width_to_track_units(right_width);
  lane_runtime->road_left = left_bound;
  lane_runtime->road_right = right_bound;
  lane_runtime->road_width = right_bound - left_bound;

  sample_road_slope_geometry(&playback->slope_geometry_state,
                             lane_runtime->edge_margins,
                             lane_runtime->edge_depth_offsets,
                             lane_runtime->edge_resource_ids);

  lane_runtime->shoulder_left = lane_runtime->road_left -
                                lane_runtime->edge_margins[ROAD_EDGE_LEFT];
  lane_runtime->shoulder_right = lane_runtime->road_right +
                                 lane_runtime->edge_margins[ROAD_EDGE_RIGHT];

  populate_road_terrain_profile(&playback->terrain_state, lane_runtime);

  surface_flags =
    get_road_section_surface_flags(&playback->section_state);
  lane_runtime->surface_flags = (unsigned char)surface_flags;
  get_road_section_collision_attributes(
    &playback->section_state, &lane_runtime->collision_kind,
    &lane_runtime->surface_selector, &lane_runtime->collision_inset,
    &lane_runtime->edge_collision_flags);

  if(lane_runtime->collision_kind == ROAD_COLLISION_KIND_LEVEL_GATE)
    {
      selected_level = (int)lane_runtime->surface_selector - 1;
      profile_index = race_mode_state.active_player_index;
      if(selected_level == player_profiles[profile_index].level)
        {
          lane_runtime->topology_flags =
            (unsigned char)(lane_runtime->topology_flags |
                            ROAD_TOPOLOGY_LEVEL_MATCH_FLAG);
        }
    }

  if((lane_runtime->surface_flags &
      ROAD_SURFACE_GEOMETRY_OVERRIDE_FLAG) != 0)
    {
      lane_runtime->geometry_mode = ROAD_GEOMETRY_MODE_CURVED;
    }
  else if(surface_flags != 0 &&
          lane_runtime->geometry_mode ==
          ROAD_GEOMETRY_MODE_PROFILE)
    {
      for(profile_index = 0;
          profile_index < ROAD_SURFACE_PROFILE_POINT_COUNT;
          profile_index++)
        {
          lane_runtime->surface_profile[profile_index].y = 0;
        }
    }

  right_bound = lane_runtime->shoulder_right;
  left_bound = lane_runtime->shoulder_left;
  if(lane_runtime->geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      if(lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].height > 0)
        {
          if(lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height > 0)
            {
              lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height = 0;
            }
          left_bound = left_bound * ROAD_BOUND_REFLECTION_SCALE -
                       lane_runtime->road_left;
          if(lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].height <
             ROAD_EDGE_MIN_EXTENSION)
            {
              lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].height =
                ROAD_EDGE_MIN_EXTENSION;
            }
        }
      else
        {
          right_bound = right_bound * ROAD_BOUND_REFLECTION_SCALE -
                        lane_runtime->road_right;
          if(lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height <
             ROAD_EDGE_MIN_EXTENSION)
            {
              lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height =
                ROAD_EDGE_MIN_EXTENSION;
            }
        }
    }

  lane_runtime->edge_shapes[ROAD_EDGE_LEFT].inner_offset =
    left_bound -
    lane_runtime->edge_shapes[ROAD_EDGE_LEFT].inner_offset;
  lane_runtime->edge_shapes[ROAD_EDGE_LEFT].outer_offset =
    left_bound -
    lane_runtime->edge_shapes[ROAD_EDGE_LEFT].outer_offset;
  lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].inner_offset =
    right_bound +
    lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].inner_offset;
  lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].outer_offset =
    right_bound +
    lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].outer_offset;

  geometry_mode = lane_runtime->geometry_mode;
  if(geometry_mode == ROAD_GEOMETRY_MODE_CURVED)
    {
      lane_runtime->edge_margins[ROAD_EDGE_LEFT] = halve_towards_zero(
        lane_runtime->edge_margins[ROAD_EDGE_LEFT]);
      lane_runtime->edge_margins[ROAD_EDGE_RIGHT] = halve_towards_zero(
        lane_runtime->edge_margins[ROAD_EDGE_RIGHT]);
      lane_runtime->shoulder_left = lane_runtime->road_left -
                                    lane_runtime->edge_margins[ROAD_EDGE_LEFT];
      lane_runtime->shoulder_right = lane_runtime->road_right +
                                     lane_runtime->edge_margins[ROAD_EDGE_RIGHT];
      if(lane_runtime->edge_depth_offsets[ROAD_EDGE_LEFT] == 0)
        {
          lane_runtime->edge_depth_offsets[ROAD_EDGE_RIGHT] =
            ROAD_EDGE_DEFAULT_DEPTH;
          lane_runtime->edge_depth_offsets[ROAD_EDGE_LEFT] =
            ROAD_EDGE_DEFAULT_DEPTH;
        }
    }
  else if(geometry_mode == ROAD_GEOMETRY_MODE_EDGE)
    {
      lane_runtime->edge_depth_offsets[ROAD_EDGE_RIGHT] = 0;
      lane_runtime->edge_depth_offsets[ROAD_EDGE_LEFT] = 0;
    }

  if(lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height <= 0)
    {
      lane_runtime->edge_shapes[ROAD_EDGE_LEFT].height =
        -lane_runtime->path_elevation;
    }
  if(lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].height <= 0)
    {
      lane_runtime->edge_shapes[ROAD_EDGE_RIGHT].height =
        -lane_runtime->path_elevation;
    }

  populate_repeated_roadside_object_group(
    object_state, lane_runtime);
  emit_track_hazards_for_segment(clip_hazard_traversal(playback),
                                 lane_runtime);

  road_right = lane_runtime->road_right;
  road_left = lane_runtime->road_left;
  for(object_index = 0;
      object_index < lane_object_position_count(lane_runtime);
      object_index++)
    {
      object_position = lane_object_position(lane_runtime, object_index);
      if(object_position > road_left && object_position < road_right)
        {
          create_race_object_from_descriptor(
            (IntrusiveListHeader *)
            track_runtime_globals.road_segment_pool,
            &track_runtime_globals.road_object_descriptor,
            object_position << TRACK_POSITION_FRACTION_BITS,
              ROAD_OBJECT_SAMPLE_MODE, lane_runtime);
        }
    }

  if(traversal_direction != 0)
    {
      populate_roadside_object_surface_heights(
        (RoadSegmentLaneRuntime *)lane_runtime->next_links[0]);
      if(lane_resource_is_dual(lane_runtime))
        {
          populate_roadside_object_surface_heights(
            (RoadSegmentLaneRuntime *)lane_runtime->next_links[1]);
        }
      if((surface_flags & ROAD_SURFACE_TERMINAL_FLAG) != 0)
        {
          apply_track_effect_spawn_record(
            surface_activation_record(playback), lane_runtime);
        }
      return;
    }

  populate_roadside_object_surface_heights(lane_runtime);
  if((surface_flags & ROAD_SURFACE_ENTRY_FLAG) != 0 &&
     (surface_flags & ROAD_SURFACE_TERMINAL_FLAG) != 0)
    {
      apply_track_effect_spawn_record(
        surface_activation_record(playback), lane_runtime);
    }
}
