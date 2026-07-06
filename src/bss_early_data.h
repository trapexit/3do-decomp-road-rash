#ifndef ROADRASH_BSS_EARLY_DATA_H
#define ROADRASH_BSS_EARLY_DATA_H

#include "front_end_cel_runtime.h"

#define RIDER_PROFILE_COUNT 2
#define RIDER_TYPE_COUNT 10
#define CHALLENGE_PROFILE_COUNT 6
#define OPPONENT_PROGRESSION_WORDS 118
#define FRONT_END_CEL_COUNT 1012
#define AUDIO_DIRECTORY_PATH_SIZE 256
#define SPATIAL_EVENT_CAPACITY 10
#define PENDING_RACER_EVENT_CAPACITY 65
#define VISIBLE_TRACK_NODE_CAPACITY 200
#define ROAD_RENDER_NODE_COUNT 34
#define ROAD_TEXTURE_CACHE_COUNT 68
#define ROAD_TEXTURE_CACHE_WORDS 162
#define ROAD_RENDER_DEPTH_COUNT 34
#define CEL_DIMENSION_ENTRY_COUNT 512
#define CEL_PIXC_SHADE_ROWS 16
#define CEL_PIXC_SHADE_COLUMNS 32

enum RoadRenderNodeType
{
  ROAD_RENDER_NODE_SINGLE = 1,
  ROAD_RENDER_NODE_DUAL = 2
};

enum RoadTextureCacheDimensions
{
  ROAD_SURFACE_SOURCE_ROWS = 3,
  ROAD_SOURCE_SIDE_COUNT = 2,
  ROAD_EDGE_SOURCE_ROWS = 2,
  ROAD_JOIN_SOURCE_ROWS = 2,
  ROAD_PROCEDURAL_SOURCE_ROWS = 2,
  ROAD_PROCEDURAL_SOURCE_COLUMNS = 3,
  ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT = 3,
  ROAD_STATIC_OBJECT_CACHE_COUNT = 2
};

enum RoadTextureBindingSlot
{
  ROAD_TEXTURE_BINDING_SURFACE_LEFT = 0,
  ROAD_TEXTURE_BINDING_SURFACE_RIGHT = 1,
  ROAD_TEXTURE_BINDING_EDGE_LEFT = 2,
  ROAD_TEXTURE_BINDING_EDGE_RIGHT = 3,
  ROAD_TEXTURE_BINDING_OBJECT_CEL = 4,
  ROAD_TEXTURE_BINDING_REPEATED_OBJECT = 5,
  ROAD_TEXTURE_BINDING_STATIC_OBJECT_FIRST = 6,
  ROAD_TEXTURE_BINDING_SLOT_COUNT = 8
};

struct RoadSegmentLaneRuntime;
struct RoadSegmentRuntime;
struct RacerEntity;

typedef struct RoadTextureCache RoadTextureCache;
typedef struct RoadTextureBindingState RoadTextureBindingState;
typedef struct RoadRenderNode RoadRenderNode;
typedef struct RoadRenderSide RoadRenderSide;
typedef struct RoadProjectionCursor RoadProjectionCursor;

typedef int RoadRenderFixed;

/* Reconstruction-only provenance for pointers into reusable family buffers. */
typedef struct FamilyResourceBindingIdentity
{
  unsigned int selector;
  unsigned int generation;
} FamilyResourceBindingIdentity;

typedef struct RoadRenderPoint
{
  RoadRenderFixed x;
  RoadRenderFixed y;
} RoadRenderPoint;

typedef struct RoadRenderPolyline6
{
  RoadRenderPoint points[6];
} RoadRenderPolyline6;

typedef union OpponentProgressionRecord
{
  unsigned int words[OPPONENT_PROGRESSION_WORDS];
  struct
  {
    unsigned int performance_values[89];
    unsigned char row_marker;
    unsigned char current_stage;
    unsigned char progression_parameters[38];
    unsigned char selection_kind;
    unsigned char selection_alignment[3];
    int selected_reward;
    unsigned int trailing_values[17];
  } fields;
} OpponentProgressionRecord;

typedef struct FrontEndCelHierarchyEntry
{
  int resource_id;
  int parent_index;
  unsigned int child_selector;
} FrontEndCelHierarchyEntry;

typedef struct SpatialAudioEvent
{
  int event_type;
  int position_x;
  int position_y;
  int position_z;
  const void *sample_image;
  int reserved_zero;
  int forward_velocity;
} SpatialAudioEvent;

typedef struct PendingRacerEvent
{
  unsigned int racer;
  int trigger_time;
} PendingRacerEvent;

struct RoadTextureCache
{
  int resource_key;
  void *surface_sources[ROAD_SURFACE_SOURCE_ROWS][ROAD_SOURCE_SIDE_COUNT];
  void *edge_sources[ROAD_EDGE_SOURCE_ROWS][ROAD_SOURCE_SIDE_COUNT];
  void *join_sources[ROAD_JOIN_SOURCE_ROWS][ROAD_SOURCE_SIDE_COUNT];
  void *procedural_sources[ROAD_PROCEDURAL_SOURCE_ROWS]
  [ROAD_PROCEDURAL_SOURCE_COLUMNS];
  void *reserved_source;
  CansAnimationData *repeated_object_animation;
  CansAnimationFrameOutput
    repeated_object_frames[ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT];
  CansAnimationData
  *static_object_animations[ROAD_STATIC_OBJECT_CACHE_COUNT];
  CansAnimationFrameOutput
    static_object_frames[ROAD_STATIC_OBJECT_CACHE_COUNT]
  [ROAD_OBJECT_CEL_SCALE_BUCKET_COUNT];
  CansAnimationData
  *racer_auxiliary_animations[ROAD_STATIC_OBJECT_CACHE_COUNT];
};

/* Reconstruction-only ownership keys live beside the oracle-layout cache.
   The cache tail at 0x280 belongs to racer auxiliary animation pointers. */
struct RoadTextureBindingState
{
  unsigned int resource_generations[ROAD_TEXTURE_BINDING_SLOT_COUNT];
  unsigned int surface_resources_bound;
  unsigned int surface_geometry_mode;
  unsigned char resource_selectors[ROAD_TEXTURE_BINDING_SLOT_COUNT];
};

/* Exact 0x104-byte projected representation of one visible road side. */
struct RoadRenderSide
{
  RoadRenderSide *next_sides[2];
  RoadRenderNode *owner_node;
  struct RoadSegmentLaneRuntime *lane;
  RoadRenderFixed world_x;
  RoadRenderFixed world_y;
  int projection_depth;
  int inverse_world_x_step;
  RoadRenderFixed projection_scale;
  RoadRenderFixed projected_lane_unit;
  RoadRenderFixed connection_geometry[8];
  RoadRenderFixed projection_origin_x;
  RoadRenderFixed horizon_y;
  RoadRenderPoint lane_edges[4];
  RoadRenderPoint mode_geometry[4];
  RoadRenderFixed transition_geometry[4];
  RoadRenderPoint attachment_points[2];
  RoadRenderFixed projected_path_elevation;
  RoadRenderPolyline6 curve_profile;
  RoadTextureBindingState *texture_binding_state;
  unsigned int reserved_e8[3];
  RoadRenderPoint curve_control;
  RoadTextureCache *texture_cache;
  unsigned char active;
  unsigned char alignment[3];
};

/* Ten-word cursor advanced once for every projected road segment. */
struct RoadProjectionCursor
{
  RoadRenderFixed world_y_step;
  RoadRenderFixed world_y;
  RoadRenderFixed world_x_step;
  RoadRenderFixed world_x;
  int projection_depth;
  RoadRenderFixed projection_origin_x;
  RoadRenderFixed projection_origin_y;
  RoadRenderFixed horizon_y;
  RoadRenderSide *left_attachment;
  RoadRenderSide *right_attachment;
};

struct RoadRenderNode
{
  struct RoadSegmentRuntime *segment;
  int node_type;
  int depth;
  unsigned char visible;
  unsigned char visibility_alignment[3];
  int sort_depth;
  RoadRenderSide *left_side;
  RoadRenderSide *right_side;
  RoadRenderSide sides[2];
};

int
render_road_center_fill(RoadRenderNode *node);
int
render_road_node_center_geometry(RoadRenderNode *node);
int
render_road_node_surfaces(RoadRenderNode *node);
int
render_projected_road_scene(void);
void
render_road_frame(void);
int
project_road_surface_profile(RoadRenderSide *road_side);
void
populate_raised_road_surface_resource_cache(RoadRenderSide *road_side);

typedef struct CelAnimationBinding
{
  int entry_index;
  unsigned char channel;
  unsigned char horizontal_variant_count;
  unsigned char vertical_variant_count;
  unsigned char horizontal_extent;
  unsigned char vertical_extent;
  unsigned char uses_variant_grid;
  signed char previous_entry_offset;
  signed char next_entry_offset;
  signed char duration_ticks;
  unsigned char transition_command;
  unsigned short alignment;
  const int *frame_flags;
} CelAnimationBinding;

extern OpponentProgressionRecord
  gRiderProgressionProfiles[RIDER_PROFILE_COUNT][RIDER_TYPE_COUNT];
extern OpponentProgressionRecord
  gChallengeProgressionProfiles[CHALLENGE_PROFILE_COUNT];
extern FrontEndCelHierarchyEntry
  gFrontEndCelHierarchy[FRONT_END_CEL_COUNT];
extern char gAudioDirectoryPath[AUDIO_DIRECTORY_PATH_SIZE];
extern SpatialAudioEvent gSpatialEventQueue[SPATIAL_EVENT_CAPACITY];
extern PendingRacerEvent
  gPendingRacerEvents[PENDING_RACER_EVENT_CAPACITY];
extern struct RacerEntity *gVisibleTrackNodes[VISIBLE_TRACK_NODE_CAPACITY];
extern RoadRenderNode gRoadRenderNodes[ROAD_RENDER_NODE_COUNT];
extern RoadTextureCache
  gRoadTextureCachePool[ROAD_TEXTURE_CACHE_COUNT];
extern RoadTextureCache gRoadTextureCacheTemplate;
extern RoadTextureBindingState
  gRoadTextureBindingStates[ROAD_TEXTURE_CACHE_COUNT];
extern int gRoadRenderDepths[ROAD_RENDER_DEPTH_COUNT];
extern CelAnimationBinding gPlayerOneFrontFlipBindings[7];
extern CelAnimationBinding gPlayerOneRollBindings[8];
extern CelAnimationBinding gPlayerOneRunBindings[6];
extern CelAnimationBinding gPlayerTwoFrontFlipBindings[7];
extern unsigned int
  gCelDimensionPreambleTable[CEL_DIMENSION_ENTRY_COUNT];
extern unsigned int
  gCelPixcShadingTable[CEL_PIXC_SHADE_ROWS][CEL_PIXC_SHADE_COLUMNS];

#endif
