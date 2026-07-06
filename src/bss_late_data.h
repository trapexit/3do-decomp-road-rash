#ifndef ROADRASH_BSS_LATE_DATA_H
#define ROADRASH_BSS_LATE_DATA_H

#include "event.h"
#include "stdio.h"
#include "track_traversal_runtime.h"

#define ROAD_PROCEDURAL_PROFILE_COUNT 6
#define ROAD_SURFACE_PROFILE_POINT_COUNT 6
#define ROAD_PROCEDURAL_POINT_COUNT 1024
#define ROAD_SEGMENT_COUNT 128
#define ROAD_PARTICLE_SENTINEL_INDEX 122
#define ROAD_PARTICLE_SLOT_COUNT 100
#define ROAD_SURFACE_STYLE_COUNT 9
#define ROAD_SURFACE_CEL_VARIANTS 3
#define AUDIO_OBJECT_SLOT_COUNT 32
#define ROAD_CLIP_PLAYBACK_BYTES 0x1F4
#define ROAD_TRACK_TRAVERSAL_BYTES 0x410

/* A pair of smoothly generated values used by the road variation profiles. */
typedef struct RoadProceduralCurvePoint
{
  int longitudinal_value;
  int lateral_value;
} RoadProceduralCurvePoint;

typedef struct RoadProceduralCurveProfile
{
  RoadProceduralCurvePoint points[ROAD_PROCEDURAL_POINT_COUNT];
} RoadProceduralCurveProfile;

enum RoadClipTraversalWordCounts
{
  ROAD_RLAN_TRAVERSAL_WORD_COUNT = 7,
  ROAD_FAMILY_SCHEDULE_TRAVERSAL_WORD_COUNT = 5,
  ROAD_TERRAIN_VARIANT_STATE_WORD_COUNT = 60,
  ROAD_CLIP_RESERVED_TAIL_WORD_COUNT = 5
};

enum RoadTerrainResourceTagValue
{
  ROAD_TERRAIN_TAG_RBLD = 0x52424C44,
  ROAD_TERRAIN_TAG_RDWD = 0x52445744,
  ROAD_TERRAIN_TAG_RHIL = 0x5248494C,
  ROAD_TERRAIN_TAG_RMTN = 0x524D544E
};

enum RoadTerrainLayoutValue
{
  ROAD_TERRAIN_SIDE_LEFT = 0,
  ROAD_TERRAIN_SIDE_RIGHT = 1,
  ROAD_TERRAIN_SIDE_COUNT = 2,
  ROAD_HILL_TERRAIN_CONTROL_COUNT = 4,
  ROAD_HILL_TERRAIN_RENDER_POINT_COUNT = 3
};

typedef unsigned char
  RoadTerrainSurfaceSample[ROAD_TERRAIN_SIDE_COUNT];

typedef struct RoadTerrainResourceHeader
{
  unsigned int tag;
  unsigned int byte_size;
} RoadTerrainResourceHeader;

typedef struct RoadTerrainSurfaceResource
{
  unsigned int tag;
  unsigned int byte_size;
  int sample_count;
  unsigned int reserved_0c;
  RoadTerrainSurfaceSample samples[1];
} RoadTerrainSurfaceResource;

typedef struct RoadTerrainEdgeEntry
{
  unsigned int start_sample;
  unsigned char reserved_04;
  unsigned char inner_step_radius;
  unsigned char inner_midpoint;
  unsigned char outer_step_radius;
  unsigned char outer_midpoint_raw;
  unsigned char slope_width_raw;
  unsigned char height_step_radius;
  unsigned char height_midpoint_raw;
  int base_height;
} RoadTerrainEdgeEntry;

typedef struct RoadTerrainEdgeResource
{
  unsigned int tag;
  unsigned int byte_size;
  int sample_count;
  unsigned int entry_counts[ROAD_TERRAIN_SIDE_COUNT];
  unsigned int reserved_14;
  RoadTerrainEdgeEntry entries[1];
} RoadTerrainEdgeResource;

typedef struct RoadHillTerrainControl
{
  unsigned char minimum_x;
  unsigned char maximum_x;
  unsigned char x_step_radius;
  signed char minimum_y;
  signed char maximum_y;
  unsigned char y_step_radius;
  unsigned char minimum_run;
  unsigned char maximum_run;
} RoadHillTerrainControl;

typedef struct RoadHillTerrainDefinition
{
  int start_sample;
  int end_sample;
  RoadTerrainSurfaceSample surface_resource_ids;
  unsigned char reserved_0a[2];
  RoadHillTerrainControl
    controls[ROAD_TERRAIN_SIDE_COUNT]
  [ROAD_HILL_TERRAIN_CONTROL_COUNT];
} RoadHillTerrainDefinition;

typedef struct RoadHillTerrainResource
{
  unsigned int tag;
  unsigned int byte_size;
  int definition_count;
  unsigned int reserved_0c;
  unsigned int reserved_10;
  RoadHillTerrainDefinition definitions[1];
} RoadHillTerrainResource;

typedef struct RoadHillTerrainPoint
{
  int x;
  int y;
} RoadHillTerrainPoint;

typedef struct RoadTerrainEdgeVariantState
{
  unsigned int reserved_14;
  int active_entry_indices[ROAD_TERRAIN_SIDE_COUNT];
  const RoadTerrainEdgeEntry
  *active_entries[ROAD_TERRAIN_SIDE_COUNT];
  int inner_offsets[ROAD_TERRAIN_SIDE_COUNT];
  int height_adjustments[ROAD_TERRAIN_SIDE_COUNT];
  int outer_adjustments[ROAD_TERRAIN_SIDE_COUNT];
} RoadTerrainEdgeVariantState;

typedef struct RoadHillTerrainVariantState
{
  int definition_index;
  unsigned int reserved_18[2];
  const void *unused_shape_entries[ROAD_TERRAIN_SIDE_COUNT];
  unsigned int reserved_28[6];
  const RoadHillTerrainDefinition *current_definition;
  int interval_starts[ROAD_TERRAIN_SIDE_COUNT]
  [ROAD_HILL_TERRAIN_CONTROL_COUNT];
  int interval_ends[ROAD_TERRAIN_SIDE_COUNT]
  [ROAD_HILL_TERRAIN_CONTROL_COUNT];
  RoadHillTerrainPoint
    current_points[ROAD_TERRAIN_SIDE_COUNT]
  [ROAD_HILL_TERRAIN_CONTROL_COUNT];
  RoadHillTerrainPoint
    interpolation_steps[ROAD_TERRAIN_SIDE_COUNT]
  [ROAD_HILL_TERRAIN_CONTROL_COUNT];
} RoadHillTerrainVariantState;

typedef union RoadTerrainVariantState
{
  unsigned int words[ROAD_TERRAIN_VARIANT_STATE_WORD_COUNT];
  RoadTerrainEdgeVariantState edge;
  RoadHillTerrainVariantState hill;
} RoadTerrainVariantState;

typedef struct RoadTerrainTraversalState
{
  const RoadTerrainResourceHeader *resource;
  unsigned int resource_tag;
  const RoadTerrainSurfaceSample *surface_resource_samples;
  int track_position;
  int sample_index;
  RoadTerrainVariantState variant;
} RoadTerrainTraversalState;

void
initialize_hill_road_terrain_variant_state(RoadTerrainTraversalState     *terrain_state,
                                           const RoadHillTerrainResource *resource,
                                           int                            initial_position);
void
initialize_road_terrain_traversal(RoadTerrainTraversalState       *terrain_state,
                                  const RoadTerrainResourceHeader *resource,
                                  int                              initial_position);
RoadTerrainTraversalState *
create_road_terrain_traversal(RoadTerrainTraversalState       *terrain_state,
                              const RoadTerrainResourceHeader *resource);
void
advance_road_terrain_edge_traversal(RoadTerrainTraversalState *terrain_state,
                                    int                        delta);
void
schedule_hill_road_terrain_interpolation(RoadTerrainTraversalState *terrain_state,
                                         int                        sample,
                                         int                        direction);
void
advance_hill_road_terrain_traversal(RoadTerrainTraversalState *terrain_state,
                                    int                        direction);
void
advance_road_terrain_traversal(RoadTerrainTraversalState *terrain_state,
                               int                        delta);

typedef struct RoadClipPlaybackState
{
  int lane_index;
  RoadTrackGraphNode *active_clip;
  int clip_position;
  int center_position;
  RoadRlanTraversalState lane_width_state;
  RoadPathTraversalState path_state;
  RoadRsldTraversalState slope_geometry_state;
  RoadSectionTraversalState section_state;
  RoadFamilyScheduleTraversalState family_schedule_state;
  RoadObjectTraversalState object_state;
  RoadTerrainTraversalState terrain_state;
  TrackHazardTraversal hazard_traversal;
  unsigned int reserved_tail[ROAD_CLIP_RESERVED_TAIL_WORD_COUNT];
} RoadClipPlaybackState;

typedef struct RoadTrackTraversalState
{
  int track_position;
  int active_channel;
  unsigned char selector_override_active;
  unsigned char clip_boundary_crossed;
  unsigned char traversal_direction;
  unsigned char transition_status;
  int junction_progress;
  RoadTrackGraphNode *active_transition;
  int right_step;
  int left_step;
  int channel_count;
  int main_fork_channel;
  int alternate_fork_channel;
  RoadClipPlaybackState channels[ROAD_TRACK_CHANNEL_COUNT];
} RoadTrackTraversalState;

struct RoadTrackTransitionRuntime
{
  RoadTrackTraversalState traversal;
  int branch_clearance_index;
};

void
initialize_track_clip_traversal(RoadClipPlaybackState *playback,
                                RoadTrackGraphNode    *clip,
                                int                    forward);
void
initialize_road_clip_playback_state(RoadClipPlaybackState *playback,
                                    RoadTrackGraphNode    *initial_clip,
                                    int                    lane_index,
                                    int                    traversal_direction);
RoadTrackTraversalState *
create_road_track_traversal(RoadTrackTraversalState *traversal,
                            RoadTrackGraphNode      *initial_clip,
                            int                      traversal_direction);
RoadClipAdvanceResult
advance_track_clip_traversal(RoadClipPlaybackState *playback,
                             int                    delta);
void
seek_track_clip_traversal(RoadClipPlaybackState *playback,
                          int                    target_position);
void
commit_selected_road_track_channel(RoadTrackTraversalState *traversal);
void
set_road_track_selector_override_enabled(RoadTrackTraversalState *traversal,
                                         int                      enabled);
int
advance_road_track_traversal(RoadTrackTraversalState *traversal,
                             int                      delta);
void
seek_road_track_traversal(RoadTrackTraversalState *traversal,
                          int                      target_position);
int
find_active_level_finish_sample(const RoadSectionResource *resource);
void *
initialize_road_section_traversal(RoadSectionTraversalState *traversal,
                                  RoadSectionResource       *resource,
                                  int                        forward);
RoadSectionTraversalState *
create_road_section_traversal(RoadSectionTraversalState *traversal,
                              RoadSectionResource       *resource);
void
advance_road_section_traversal(RoadSectionTraversalState *traversal,
                               int                        delta);
int
get_road_section_surface_flags(const RoadSectionTraversalState *traversal);
void
get_road_section_collision_attributes(const RoadSectionTraversalState *traversal,
                                      unsigned char                   *out_collision_kind,
                                      unsigned char                   *out_selector,
                                      signed char                     *out_collision_inset,
                                      unsigned char                   *out_edge_collision_flags);

typedef struct RoadEdgeShape
{
  int inner_offset;
  int outer_offset;
  int height;
} RoadEdgeShape;

typedef struct RoadLanePoint
{
  int x;
  int y;
} RoadLanePoint;

enum RoadsideObjectLayoutValue
{
  ROADSIDE_STATIC_OBJECT_COUNT = 2,
  ROAD_OBJECT_COLLISION_BOX_COUNT = 2,
  ROAD_OBJECT_PLACEMENT_COUNT = 8
};

enum RoadObjectPlacementFlagValue
{
  ROAD_OBJECT_PLACEMENT_MIRRORED = 1,
  ROAD_OBJECT_PLACEMENT_FACES_RIGHT = 2,
  ROAD_OBJECT_PLACEMENT_COLLISION_BOXES_CACHED = 4
};

typedef struct RoadObjectBounds
{
  int left;
  int top;
  int right;
  int bottom;
} RoadObjectBounds;

typedef struct StaticRoadsideObject
{
  unsigned char resource_selector;
  unsigned char flags;
  unsigned char visibility_group;
  unsigned char collision_box_count;
  RoadObjectBounds bounds[ROAD_OBJECT_COLLISION_BOX_COUNT];
  int lateral_position;
  int surface_height;
} StaticRoadsideObject;

typedef struct RepeatedRoadsideObjectGroup
{
  unsigned char placement_count;
  unsigned char collision_box_count;
  unsigned char scale;
  unsigned char resource_selector;
  unsigned char flags;
  unsigned char alignment[3];
  RoadObjectBounds bounds[ROAD_OBJECT_COLLISION_BOX_COUNT];
  int lateral_positions[ROAD_OBJECT_PLACEMENT_COUNT];
  int surface_heights[ROAD_OBJECT_PLACEMENT_COUNT];
} RepeatedRoadsideObjectGroup;

/* One rendered lane within a 0x390-byte road segment runtime record. */
typedef struct RoadSegmentLaneRuntime
{
  RoadTrackGraphNode *clip_node;
  int clip_position;
  struct RoadSegmentLaneRuntime *previous_links[4];
  struct RoadSegmentLaneRuntime *next_links[4];
  void *resource_handle;
  int right_width;
  int left_width;
  unsigned char right_width_cells;
  unsigned char left_width_cells;
  unsigned char topology_flags;
  unsigned char surface_flags;
  unsigned char collision_kind;
  unsigned char surface_selector;
  signed char collision_inset;
  unsigned char edge_collision_flags;
  unsigned char geometry_mode;
  unsigned char static_object_count;
  unsigned char object_alignment[2];
  StaticRoadsideObject
    static_objects[ROADSIDE_STATIC_OBJECT_COUNT];
  RepeatedRoadsideObjectGroup repeated_objects;
  int curvature_projection_step;
  int path_curvature_step;
  int path_elevation_step;
  int path_elevation;
  int road_width;
  int elevation_projection_step;
  int center_position;
  int segment_left_bound;
  int segment_right_bound;
  int connected_left_bound;
  int connected_right_bound;
  int road_left;
  int road_right;
  int shoulder_left;
  int shoulder_right;
  RoadLanePoint surface_profile[ROAD_SURFACE_PROFILE_POINT_COUNT];
  int profile_shading[6];
  int edge_margins[ROAD_TRACK_CHANNEL_COUNT];
  int edge_depth_offsets[2];
  unsigned char surface_resource_ids[2];
  unsigned char edge_resource_ids[2];
  RoadEdgeShape edge_shapes[2];
} RoadSegmentLaneRuntime;

void
populate_repeated_roadside_object_group(const RoadObjectTraversalState *object_state,
                                        RoadSegmentLaneRuntime         *lane_runtime);
void
populate_roadside_object_surface_heights(RoadSegmentLaneRuntime *lane_runtime);
int
sample_road_cross_section_height(const RoadSegmentLaneRuntime *lane_runtime,
                                 int                           lateral_position,
                                 unsigned int                  interpolation_fraction);

void
populate_road_terrain_surface_resources(const RoadTerrainTraversalState *terrain_state,
                                        RoadSegmentLaneRuntime          *lane_runtime);
void
populate_road_terrain_edge_shapes(const RoadTerrainTraversalState *terrain_state,
                                  RoadSegmentLaneRuntime          *lane_runtime);
void
populate_hill_road_terrain_profile(const RoadTerrainTraversalState *terrain_state,
                                   RoadSegmentLaneRuntime          *lane_runtime);
int
populate_road_terrain_profile(const RoadTerrainTraversalState *terrain_state,
                              RoadSegmentLaneRuntime          *lane_runtime);

typedef struct RoadSegmentRuntime
{
  struct RoadSegmentRuntime *next;
  struct RoadSegmentRuntime *previous;
  int track_position;
  RoadSegmentLaneRuntime *selected_lanes[4];
  unsigned char lane_count;
  signed char orientation;
  signed char transition_amount;
  unsigned char left_transition_profile_count;
  unsigned char right_transition_profile_count;
  unsigned char transition_profile_alignment[3];
  int left_step;
  int right_step;
  int transition_type;
  RoadSegmentLaneRuntime lanes[2];
} RoadSegmentRuntime;

typedef struct RoadParticleSlot
{
  int horizontal_phase;
  int vertical_phase;
  int half_width;
  int half_height;
  unsigned int active_flags;
  void *cel;
} RoadParticleSlot;

/*
 * The final six entries of the 128-pointer road index are intentionally
 * overlaid by the 0x18-byte particle sentinel used by the road particle pass.
 * Both
 * views then agree that the 100 particle records begin at byte offset 0x200.
 */
typedef struct RoadSegmentIndexView
{
  RoadSegmentRuntime *by_track_index[ROAD_SEGMENT_COUNT];
  RoadParticleSlot particle_slots[ROAD_PARTICLE_SLOT_COUNT];
} RoadSegmentIndexView;

typedef struct RoadParticleSentinelView
{
  RoadSegmentRuntime *leading_track_index[ROAD_PARTICLE_SENTINEL_INDEX];
  RoadParticleSlot particle_sentinel;
  RoadParticleSlot particle_slots[ROAD_PARTICLE_SLOT_COUNT];
} RoadParticleSentinelView;

typedef union RoadSegmentEffectWorkspace
{
  RoadSegmentIndexView segment_index;
  RoadParticleSentinelView particle_effects;
} RoadSegmentEffectWorkspace;

extern RoadProceduralCurveProfile
  gRoadProceduralCurveProfiles[ROAD_PROCEDURAL_PROFILE_COUNT];
extern RoadTrackTraversalState gRoadTrackForwardTraversal;
extern RoadTrackTraversalState gRoadTrackReverseTraversal;
extern RoadSegmentRuntime gRoadSegments[ROAD_SEGMENT_COUNT];
extern RoadSegmentEffectWorkspace gRoadSegmentEffectWorkspace;
extern void *gRoadSurfaceCelVariants[ROAD_SURFACE_STYLE_COUNT]
[ROAD_SURFACE_CEL_VARIANTS];
extern char gAudioWorkingDirectory[256];
extern void *gLoadedAudioObjects[AUDIO_OBJECT_SLOT_COUNT];
extern ConfigurationRequest gEventUtilityConfiguration;
extern FILE gStandardInputStream;
extern FILE gStandardOutputStream;
extern FILE gStandardErrorStream;

int *
initialize_road_segment_lane_defaults(RoadSegmentLaneRuntime *lane_runtime,
                                      int                     unused_callback_index,
                                      int                     unused_callback_direction);
void
reconcile_linked_road_lane_surface_profile(RoadSegmentLaneRuntime *source_lane,
                                           RoadSegmentLaneRuntime *linked_lane);

#endif
