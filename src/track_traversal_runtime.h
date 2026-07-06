#ifndef ROADRASH_TRACK_TRAVERSAL_RUNTIME_H
#define ROADRASH_TRACK_TRAVERSAL_RUNTIME_H

enum TrackTraversalConstants
{
  TRACK_POSITION_FRACTION_BITS = 8,
  TRACK_POSITION_UNIT = 1 << TRACK_POSITION_FRACTION_BITS,
  TRACK_POSITION_FRACTION_MASK = TRACK_POSITION_UNIT - 1
};

enum TrackTraversalDirectionValue
{
  TRACK_TRAVERSAL_REVERSE = 0,
  TRACK_TRAVERSAL_FORWARD = 1
};

enum RoadPathResourceValue
{
  ROAD_PATH_RESOURCE_TAG = 0x52505448,
  ROAD_PATH_CURVATURE_STEP_INDEX = 0,
  ROAD_PATH_ELEVATION_STEP_INDEX = 1,
  ROAD_PATH_SAMPLE_VALUE_COUNT = 2,
  ROAD_PATH_DEFAULT_SAMPLE_COUNT = 0x14C8
};

/* ARM SDT pads two-byte structs, so keep the on-disc sample as an array. */
typedef signed char RoadPathSample[ROAD_PATH_SAMPLE_VALUE_COUNT];

typedef struct RoadPathResource
{
  unsigned int tag;
  unsigned int byte_size;
  int sample_count;
  signed short start_elevation;
  signed short end_elevation;
  RoadPathSample samples[1];
} RoadPathResource;

typedef struct RoadPathTraversalState
{
  const RoadPathResource *resource;
  int elevation;
  int track_position;
} RoadPathTraversalState;

/*
 * These generic cursor formats are linked into the executable but have no
 * callers outside this self-contained cluster and no matching live Road Rash
 * course chunk.  Keep neutral names rather than assigning either format to an
 * RSGS optional slot.
 */
typedef struct TrackBoundaryEntry
{
  int sample_index;
  unsigned int reserved_04;
} TrackBoundaryEntry;

typedef struct TrackBoundaryTable
{
  unsigned int reserved_00;
  unsigned int reserved_04;
  int entry_count;
  unsigned int reserved_0c;
  unsigned int reserved_10;
  TrackBoundaryEntry entries[1];
} TrackBoundaryTable;

typedef struct TrackBoundaryCursor
{
  const TrackBoundaryTable *table;
  int track_position;
  int boundary_index;
  int current_sample;
  int previous_sample;
} TrackBoundaryCursor;

typedef struct SignedTrackSampleSeries
{
  int terminal_sample_index;
  const signed char *samples;
} SignedTrackSampleSeries;

typedef struct SignedTrackSampleCursor
{
  const SignedTrackSampleSeries *series;
  int track_position;
} SignedTrackSampleCursor;

enum RoadInterpolatedResourceValue
{
  ROAD_RLAN_RESOURCE_TAG = 0x524C414E,
  ROAD_RSLD_RESOURCE_TAG = 0x52534C44,
  ROAD_INTERPOLATED_VALUE_FRACTION_BITS = 16
};

typedef struct RoadRlanEntry
{
  int start_sample;
  int end_sample;
  unsigned char start_left_width;
  unsigned char start_right_width;
  unsigned char end_left_width;
  unsigned char end_right_width;
  int left_width_step;
  int right_width_step;
} RoadRlanEntry;

typedef struct RoadRlanResource
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  unsigned int reserved_0c;
  RoadRlanEntry entries[1];
} RoadRlanResource;

typedef struct RoadRlanTraversalState
{
  const RoadRlanResource *resource;
  int end_position;
  int track_position;
  int entry_index;
  const RoadRlanEntry *current_entry;
  int left_width_accumulator;
  int right_width_accumulator;
} RoadRlanTraversalState;

typedef struct RoadRsldEntry
{
  int start_sample;
  int end_sample;
  unsigned char start_left_margin;
  unsigned char start_right_margin;
  unsigned char end_left_margin;
  unsigned char end_right_margin;
  unsigned char start_left_depth;
  unsigned char start_right_depth;
  unsigned char end_left_depth;
  unsigned char end_right_depth;
  unsigned char edge_resource_ids[2];
  unsigned char reserved_12[2];
  int left_margin_step;
  int right_margin_step;
  int left_depth_step;
  int right_depth_step;
} RoadRsldEntry;

typedef struct RoadRsldResource
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  unsigned int reserved_0c;
  RoadRsldEntry entries[1];
} RoadRsldResource;

typedef struct RoadRsldTraversalState
{
  const RoadRsldResource *resource;
  int end_position;
  int track_position;
  int entry_index;
  const RoadRsldEntry *current_entry;
  int left_margin_accumulator;
  int right_margin_accumulator;
  int left_depth_accumulator;
  int right_depth_accumulator;
} RoadRsldTraversalState;

enum RoadFamilyScheduleValue
{
  ROAD_FAMILY_SCHEDULE_TAG_RRSM = 0x5252534D,
  ROAD_FAMILY_SCHEDULE_FORWARD_BOUNDARY = 1,
  ROAD_FAMILY_SCHEDULE_REVERSE_BOUNDARY = 2,
  ROAD_FAMILY_SCHEDULE_FORWARD_PREFETCH_REQUESTED = 3,
  ROAD_FAMILY_SCHEDULE_REVERSE_PREFETCH_REQUESTED = 4,
  ROAD_FAMILY_SCHEDULE_FORWARD_WINDOW_BASE = 88,
  ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_BASE = 40,
  ROAD_FAMILY_SCHEDULE_FORWARD_WINDOW_SPAN_SHIFT = 1,
  ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_SPAN_SHIFT = 2,
  ROAD_FAMILY_SCHEDULE_REVERSE_WINDOW_ROUND_BIAS = 3
};

/* One mutable family-resource interval boundary in an RRSM track resource. */
typedef struct RoadFamilyScheduleEntry
{
  int sample_index;
  unsigned char boundary_state;
  unsigned char family_group_index;
  unsigned short reserved_06;
  unsigned int packed_family_id;
} RoadFamilyScheduleEntry;

typedef struct RoadFamilyScheduleResource
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  unsigned char family_group_count;
  unsigned char family_table_size_kib;
  unsigned char forward_schedule_bias;
  unsigned char reserved_0f;
  unsigned int reserved_10;
  unsigned int reserved_14;
  RoadFamilyScheduleEntry entries[1];
} RoadFamilyScheduleResource;

typedef struct RoadFamilyScheduleTraversalState
{
  RoadFamilyScheduleResource *resource;
  int track_position;
  int entry_index;
  int sample_index;
  unsigned char traversal_direction;
  unsigned char alignment[3];
} RoadFamilyScheduleTraversalState;

enum RoadObjectResourceValue
{
  ROAD_OBJECT_RESOURCE_TAG = 0x52524F42,
  ROAD_OBJECT_DISABLED_SELECTOR = 0x3F
};

enum RoadObjectEntryValue
{
  ROAD_OBJECT_ALTERNATE_SIDES_SHIFT = 31,
  ROAD_OBJECT_MIRROR_SPRITE_SHIFT = 30,
  ROAD_OBJECT_WIDE_SPACING_SHIFT = 29,
  ROAD_OBJECT_BASE_LEFT_SIDE_SHIFT = 28,
  ROAD_OBJECT_ROW_COUNT_SHIFT = 24,
  ROAD_OBJECT_RESOURCE_SELECTOR_SHIFT = 16,
  ROAD_OBJECT_ROW_SPACING_SHIFT = 12,
  ROAD_OBJECT_RANDOM_LATERAL_SPACING_SHIFT = 11,
  ROAD_OBJECT_COLUMN_COUNT_SHIFT = 8,
  ROAD_OBJECT_SCALE_SHIFT = 4,
  ROAD_OBJECT_SINGLE_BIT_MASK = 1,
  ROAD_OBJECT_NIBBLE_MASK = 0x0F,
  ROAD_OBJECT_RESOURCE_SELECTOR_MASK = 0xFF,
  ROAD_OBJECT_COLUMN_COUNT_MASK = 7,
  ROAD_OBJECT_SPACING_FIELD_MASK = 0xF0,
  ROAD_OBJECT_NORMAL_SPAN_SPACING_SHIFT = 6,
  ROAD_OBJECT_LARGE_SPAN_SPACING_SHIFT = 8,
  ROAD_OBJECT_NORMAL_POINT_SPACING_SHIFT = 3,
  ROAD_OBJECT_LARGE_POINT_SPACING_SHIFT = 5,
  ROAD_OBJECT_LARGE_SPACING_BASE = 0x400
};

typedef struct RoadObjectResource
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  int sample_count;
  unsigned int reserved_10;
  unsigned int entries[1];
} RoadObjectResource;

typedef struct RoadObjectTraversalState
{
  const RoadObjectResource *resource;
  unsigned int reserved_04;
  int track_position;
  int entry_index;
  unsigned int reserved_10;
  unsigned char alternate_sides;
  unsigned char mirror_sprite;
  unsigned char wide_spacing;
  unsigned char base_left_side;
  unsigned char row_count_minus_one;
  unsigned char resource_selector;
  unsigned char reserved_1a;
  unsigned char row_spacing_minus_one;
  unsigned char randomize_lateral_spacing;
  unsigned char column_count_minus_one;
  unsigned char scale_eighths;
  unsigned char entry_runtime_flags;
  int lateral_spacing;
  int active_start_sample;
  int active_end_sample;
  int current_sample;
} RoadObjectTraversalState;

enum RoadTrackChannelIndexValue
{
  ROAD_TRACK_FIRST_CHANNEL = 0,
  ROAD_TRACK_SECOND_CHANNEL = 1,
  ROAD_TRACK_CHANNEL_COUNT = 2
};

enum RoadTrackChannelCountValue
{
  ROAD_TRACK_SINGLE_CHANNEL_COUNT = 1,
  ROAD_TRACK_DUAL_CHANNEL_COUNT = ROAD_TRACK_CHANNEL_COUNT
};

enum RoadTrackTransitionBuildValue
{
  ROAD_TRACK_TRANSITION_SAMPLE_REVERSE = -1,
  ROAD_TRACK_TRANSITION_SAMPLE_FORWARD = 1,
  ROAD_TRACK_TRANSITION_SAMPLE_COUNT = 0x51,
  ROAD_TRACK_TRANSITION_CLEARANCE_THRESHOLD = TRACK_POSITION_UNIT,
  ROAD_TRACK_TRANSITION_SCAN_SPAN =
    ROAD_TRACK_TRANSITION_SAMPLE_COUNT * TRACK_POSITION_UNIT
};

enum RoadTrackTransitionStatusValue
{
  ROAD_TRACK_TRANSITION_NONE = 0,
  ROAD_TRACK_TRANSITION_REVERSE_BRANCH_SINGLE_BOUNDARY = 1,
  ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_BOUNDARY = 2,
  ROAD_TRACK_TRANSITION_FORWARD_BRANCH_SINGLE_BOUNDARY = 3,
  ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_BOUNDARY = 4,
  ROAD_TRACK_TRANSITION_FORWARD_BRANCH_DUAL_SPAN = 5,
  ROAD_TRACK_TRANSITION_REVERSE_BRANCH_DUAL_SPAN = 6
};

typedef struct RoadTrackGraphNode RoadTrackGraphNode;
typedef struct RoadTrackTransitionRuntime RoadTrackTransitionRuntime;
typedef int RoadClipAdvanceResult;

enum RoadSectionEntryKindValue
{
  ROAD_SECTION_ENTRY_KIND_LEVEL_GATE = 6
};

typedef struct RoadSectionEntry
{
  int start_sample;
  int end_sample;
  unsigned char kind;
  signed char selector;
  signed char collision_inset;
  unsigned char edge_collision_flags;
} RoadSectionEntry;

typedef struct RoadSectionResource
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  int sample_count;
  unsigned int reserved_10;
  RoadSectionEntry entries[1];
} RoadSectionResource;

typedef struct RoadSectionTraversalState
{
  RoadSectionResource *resource;
  int position;
  int entry_index;
  unsigned char active_kind;
  unsigned char alignment[3];
  RoadSectionEntry *active_entry;
} RoadSectionTraversalState;

enum RoadClipAdvanceResultValue
{
  ROAD_CLIP_ADVANCE_WITHIN_SEGMENT = 0,
  ROAD_CLIP_ADVANCE_CROSSED_SEGMENT = 1,
  ROAD_CLIP_ADVANCE_REQUIRES_TRANSITION = 2,
  ROAD_CLIP_ADVANCE_BLOCKED = 3
};

#define TRACK_DIRECTION_BYTE_MASK 0xFF
#define TRACK_HAZARD_FRAME_SHIFT 16
#define TRACK_HAZARD_FRAME_MASK 0x7FFFU
#define TRACK_HAZARD_KIND_BYTE_MASK 0xFFU

enum TrackHazardResourceValue
{
  TRACK_HAZARD_RESOURCE_TAG = 0x52485A44,
  TRACK_HAZARD_TABLE_HEADER_BYTES = 0x10
};

typedef struct TrackHazardEntry
{
  unsigned int control_word;
  unsigned int detail_word;
} TrackHazardEntry;

/* RHZD chunk header followed by entry_count mutable entries. */
typedef struct TrackHazardTable
{
  unsigned int tag;
  unsigned int byte_size;
  int entry_count;
  unsigned int reserved_0c;
  TrackHazardEntry entries[1];
} TrackHazardTable;

/*
 * The oracle's standalone constructor allocates 0x28 bytes although only the
 * first three words are read or written by hazard traversal.  Preserve the
 * remaining allocator reserve explicitly without treating it as live state.
 */
typedef struct TrackHazardTraversal
{
  TrackHazardTable *table;
  int track_position;
  int entry_index;
  unsigned int allocator_reserve_words[7];
} TrackHazardTraversal;

int
get_track_hazard_entry_count(const TrackHazardTable *table);
int
is_road_path_resource(const RoadPathResource *resource);
int
get_road_path_sample_count(const RoadPathResource *resource);
void
initialize_road_path_traversal(RoadPathTraversalState *path_state,
                               const RoadPathResource *resource,
                               int                     forward);
RoadPathTraversalState *
create_road_path_traversal(RoadPathTraversalState *path_state,
                           const RoadPathResource *resource);
void
advance_road_path_traversal(RoadPathTraversalState *path_state,
                            int                     delta);
int
sample_road_path_steps(const RoadPathTraversalState *path_state,
                       int                          *out_curvature_step,
                       int                          *out_elevation_step);
int
get_road_path_elevation(const RoadPathTraversalState *path_state);
void
initialize_track_boundary_cursor(TrackBoundaryCursor      *cursor,
                                 const TrackBoundaryTable *table,
                                 int                       forward);
TrackBoundaryCursor *
create_track_boundary_cursor(TrackBoundaryCursor      *cursor,
                             const TrackBoundaryTable *table);
void
seek_track_boundary_index(TrackBoundaryCursor *cursor,
                          int                  forward);
void
synchronize_track_boundary_cursor(TrackBoundaryCursor *cursor);
void
advance_track_boundary_cursor(TrackBoundaryCursor *cursor,
                              int                  delta);
void
initialize_signed_track_sample_cursor(SignedTrackSampleCursor       *cursor,
                                      const SignedTrackSampleSeries *series,
                                      int                            forward);
SignedTrackSampleCursor *
create_signed_track_sample_cursor(SignedTrackSampleCursor       *cursor,
                                  const SignedTrackSampleSeries *series);
int
sample_signed_track_value(const SignedTrackSampleCursor *cursor);
void
advance_signed_track_sample_cursor(SignedTrackSampleCursor *cursor,
                                   int                      delta);
void *
initialize_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                     const RoadRlanResource *resource,
                                     int                     forward);
RoadRlanTraversalState *
create_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                 const RoadRlanResource *resource);
int
sample_road_lane_widths(const RoadRlanTraversalState *lane_state,
                        int                          *out_left_width,
                        int                          *out_right_width);
void
advance_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                                  int                     delta);
void
seek_road_lane_width_traversal(RoadRlanTraversalState *lane_state,
                               int                     target_position);
void *
initialize_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                         const RoadRsldResource *resource,
                                         int                     forward);
RoadRsldTraversalState *
create_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                     const RoadRsldResource *resource);
void
sample_road_slope_geometry(const RoadRsldTraversalState *slope_state,
                           int                          *out_margins,
                           int                          *out_depth_offsets,
                           unsigned char                *out_edge_resource_ids);
void
advance_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                      int                     delta);
void
seek_road_slope_geometry_traversal(RoadRsldTraversalState *slope_state,
                                   int                     target_position);
void
initialize_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                          RoadFamilyScheduleResource       *resource,
                                          int                               track_position);
RoadFamilyScheduleTraversalState *
create_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                      RoadFamilyScheduleResource       *resource,
                                      int                               traversal_direction);
void
process_road_family_schedule_events(RoadFamilyScheduleTraversalState *schedule_state,
                                    int                               movement_direction);
void
road_family_schedule_lane_hook(const RoadFamilyScheduleTraversalState *schedule_state);
void
advance_road_family_schedule_traversal(RoadFamilyScheduleTraversalState *schedule_state,
                                       int                               delta);
void
decode_road_object_entry(RoadObjectTraversalState *object_state,
                         int                       entry_index);
void
initialize_road_object_traversal(RoadObjectTraversalState *object_state,
                                 const RoadObjectResource *resource,
                                 int                       forward);
RoadObjectTraversalState *
create_road_object_traversal(RoadObjectTraversalState *object_state,
                             const RoadObjectResource *resource);
void
advance_road_object_traversal(RoadObjectTraversalState *object_state,
                              int                       delta);

#endif
