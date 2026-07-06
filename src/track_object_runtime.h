#ifndef ROADRASH_TRACK_OBJECT_RUNTIME_H
#define ROADRASH_TRACK_OBJECT_RUNTIME_H

#include "racer_runtime.h"

#ifndef offsetof
  #include "stddef.h"
#endif

enum TrackObjectMode
{
  TRACK_OBJECT_MODE_FORWARD = 0,
  TRACK_OBJECT_MODE_CROSS_POSITIVE = 1,
  TRACK_OBJECT_MODE_REVERSE = 2,
  TRACK_OBJECT_MODE_CROSS_NEGATIVE = 3
};

enum TrackObjectConstants
{
  TRACK_OBJECT_BITS_PER_BYTE = 8,
  TRACK_OBJECT_DIRECTION_MASK = 0xFF,
  TRACK_OBJECT_MODE_DIRECTION_BIT = 1,
  TRACK_OBJECT_MODE_COUNT = 4,
  TRACK_OBJECT_MODE_TEMPLATE_WORD_COUNT = 6,
  TRACK_OBJECT_ANIMATION_CANDIDATE_CAPACITY = 48,
  TRACK_OBJECT_PERPENDICULAR_ORIENTATION = 0x80,
  TRACK_OBJECT_LANE_SPACING = 250,
  TRACK_OBJECT_LANE_CENTER_BIAS = 32000,
  TRACK_OBJECT_POSITION_FRACTION_BITS = 8,
  TRACK_OBJECT_POSITION_UNIT = 0x100,
  TRACK_OBJECT_DISTANCE_FRACTION_BITS = 4,
  TRACK_OBJECT_LATERAL_DISTANCE_FRACTION_BITS = 12,
  TRACK_OBJECT_VELOCITY_SCALE_SHIFT_A = 7,
  TRACK_OBJECT_VELOCITY_SCALE_SHIFT_B = 9,
  TRACK_OBJECT_VELOCITY_SCALE_SHIFT_C = 12,
  TRACK_OBJECT_SIGNED_24_SHIFT = 8,
  TRACK_OBJECT_RANDOM_SPEED_MASK = 0x7F,
  TRACK_OBJECT_RANDOM_SPEED_BASE = 0xC0,
  TRACK_OBJECT_FORWARD_ANIMATION_FLAG = 0x00100000,
  TRACK_OBJECT_REVERSE_ANIMATION_FLAG = 0x00080000,
  TRACK_OBJECT_CROSS_ANIMATION_FLAG = 0x00040000,
  TRACK_OBJECT_DIRECTION_ANIMATION_FLAGS = 0x001C0000,
  TRACK_OBJECT_VISIBILITY_BITS = 0x03,
  TRACK_OBJECT_VISIBILITY_LOOKBEHIND = 0x400,
  TRACK_OBJECT_VISIBILITY_DISTANCE = 0x2800,
  TRACK_OBJECT_MINIMUM_WIDTH = 20,
  TRACK_OBJECT_WIDTH_MULTIPLIER = 2,
  TRACK_OBJECT_ROAD_MARGIN_MULTIPLIER = 3,
  TRACK_OBJECT_DEFAULT_SEARCH_DISTANCE = 1000,
  TRACK_OBJECT_TARGET_SEARCH_BASE = 0x800,
  TRACK_OBJECT_SEARCH_SCALE = 16,
  TRACK_OBJECT_NEARBY_TYPE_MASK = 0x3F,
  TRACK_OBJECT_TOPOLOGY_MATCH_MASK = 0x03,
  TRACK_OBJECT_LATERAL_SEARCH_SPAN = 0x80,
  TRACK_OBJECT_UNBOUNDED_SEARCH_DISTANCE = 0x20000000,
  TRACK_OBJECT_BLOCKING_COLLISION_FLAG = 0x10,
  TRACK_OBJECT_CROSSING_COLLISION_FLAGS = 0x0C,
  TRACK_OBJECT_SPEED_ADJUSTMENT = 300,
  TRACK_OBJECT_TARGET_LONGITUDINAL_MARGIN = 0xA0,
  TRACK_OBJECT_TARGET_LATERAL_MARGIN = 0x60,
  TRACK_OBJECT_TARGET_PASSED_MARGIN = 6,
  TRACK_OBJECT_MINIMUM_APPROACH_LIMIT = 25,
  TRACK_OBJECT_PREDICTION_SCALE = 200,
  TRACK_OBJECT_APPROACH_SCALE = 100,
  TRACK_OBJECT_DIRECTION_TABLE_BASE = 4,
  TRACK_OBJECT_DIRECTION_SCALE = 800,
  TRACK_OBJECT_CROSS_TRACK_OFFSET = 0x32000,
  TRACK_OBJECT_NORMAL_ACCELERATION_SCALE = 0x12,
  TRACK_OBJECT_FAST_ACCELERATION_SCALE = 0x24,
  TRACK_OBJECT_ANIMATION_NEAR_DISTANCE = 0x500,
  TRACK_OBJECT_ANIMATION_FAR_DISTANCE = 0xA00,
  TRACK_OBJECT_ANIMATION_NEAR = 1,
  TRACK_OBJECT_ANIMATION_FAR = 2,
  TRACK_ROAD_REVERSE_CONNECTION_DIRECTION = 1,
  TRACK_ROAD_FORWARD_CONNECTION_DIRECTION = 3,
  TRACK_OBJECT_SHORT_DELAY_TICKS = 60,
  TRACK_OBJECT_LONG_DELAY_TICKS = 240
};

typedef struct RoadJunctionRuntime
{
  unsigned char reserved_00[0x1C];
  unsigned char junction_kind;
  unsigned char reserved_1d[0x07];
  int crossing_width;
  unsigned char reserved_28[0x04];
  int connection_direction;
} RoadJunctionRuntime;

typedef struct TrackObjectPosition
{
  int track_position;
  int vertical_position;
  int auxiliary_position;
  int lateral_position;
} TrackObjectPosition;

typedef struct TrackInteractionObject
{
  PooledListNode list_node;
  unsigned char reserved_00c[0x0C];
  int time_step;
  TrackObjectPosition position;
  RoadSegmentLaneRuntime *track_segment;
  unsigned char reserved_030[0x90];
  unsigned char status_flags;
  unsigned char status_alignment[3];
  int collision_flags;
  unsigned char reserved_0c8[0x24];
  RacerEntity *motion_source;
  int interaction_selector;
  unsigned char reserved_0f4[0x14];
  RacerVelocity world_velocity;
} TrackInteractionObject;

typedef struct TrackMotionCommand
{
  int lateral_offset;
  int direction;
} TrackMotionCommand;

typedef struct TrackObjectModeInitialization
{
  int initial_vertical_position;
  int initial_track_position;
  int initial_velocity_forward;
  int initial_velocity_lateral;
  int initial_velocity_vertical;
  RacerContactCallback contact_callback;
} TrackObjectModeInitialization;

typedef struct TrackObject
{
  unsigned char reserved_000[0x18];
  int time_step;
  TrackObjectPosition position;
  RoadSegmentLaneRuntime *track_segment;
  unsigned char reserved_030[0x14];
  RacerPositionSnapshot position_snapshot;
  RacerOrientation orientation;
  RacerOrientation previous_orientation;
  RiderAnimationRuntime animation;
  RiderAnimationRuntime previous_animation;
  unsigned char reserved_0a8[0x18];
  unsigned char status_flags;
  unsigned char status_alignment[3];
  int collision_flags;
  unsigned char reserved_0c8[0x14];
  RaceObjectRenderCallback render_object;
  RaceObjectUpdateCallback prepare_update;
  RaceObjectUpdateCallback perform_update;
  RaceObjectResetCallback reset_object;
  RacerVelocity velocity;
  unsigned char reserved_0f8[0x0C];
  int lateral_velocity_sample;
  RacerVelocity world_velocity;
  unsigned char reserved_114[0x14];
  int heading;
  unsigned char reserved_12c[0x14];
  RoadSegmentLaneRuntime *primary_track;
  unsigned char reserved_144[0x30];
  int collision_half_width;
  int collision_half_length;
  unsigned char reserved_17c[0x24];
  TrackInteractionObject *contact;
  unsigned char reserved_1a4[0x1C];
  int cruise_speed;
  unsigned char animation_phase;
  unsigned char animation_color;
  unsigned char animation_variant;
  unsigned char random_alignment;
  int mode;
  RacerVelocity inherited_velocity;
  TrackMotionCommand motion_command;
  unsigned char reserved_1e0[0x04];
  signed char lane;
  unsigned char lane_alignment[3];
  TrackInteractionObject *target;
  TrackInteractionObject *nearby_object;
  unsigned char mode_update_seen;
  unsigned char mode_update_alignment[3];
  int delay_ticks;
} TrackObject;

#define TRACK_OBJECT_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

TRACK_OBJECT_LAYOUT_ASSERT(RoadJunctionKindOffsetIs1c,
                           offsetof(RoadJunctionRuntime, junction_kind) ==
                           0x1C);
TRACK_OBJECT_LAYOUT_ASSERT(RoadJunctionWidthOffsetIs24,
                           offsetof(RoadJunctionRuntime, crossing_width) ==
                           0x24);
TRACK_OBJECT_LAYOUT_ASSERT(RoadJunctionDirectionOffsetIs2c,
                           offsetof(RoadJunctionRuntime,
                                    connection_direction) == 0x2C);
TRACK_OBJECT_LAYOUT_ASSERT(RoadForwardSegmentOffsetIs08,
                           offsetof(RoadSegmentLaneRuntime,
                                    previous_links[0]) == 0x08);
TRACK_OBJECT_LAYOUT_ASSERT(RoadReverseSegmentOffsetIs18,
                           offsetof(RoadSegmentLaneRuntime,
                                    next_links[0]) == 0x18);
TRACK_OBJECT_LAYOUT_ASSERT(RoadJunctionOffsetIs28,
                           offsetof(RoadSegmentLaneRuntime,
                                    resource_handle) == 0x28);
TRACK_OBJECT_LAYOUT_ASSERT(RoadForwardLaneSpanOffsetIs2c,
                           offsetof(RoadSegmentLaneRuntime, right_width) ==
                           0x2C);
TRACK_OBJECT_LAYOUT_ASSERT(RoadReverseLaneSpanOffsetIs30,
                           offsetof(RoadSegmentLaneRuntime, left_width) ==
                           0x30);
TRACK_OBJECT_LAYOUT_ASSERT(RoadConnectionFlagsOffsetIs36,
                           offsetof(RoadSegmentLaneRuntime, topology_flags) ==
                           0x36);
TRACK_OBJECT_LAYOUT_ASSERT(RoadTraversalFlagsOffsetIs37,
                           offsetof(RoadSegmentLaneRuntime, surface_flags) ==
                           0x37);
TRACK_OBJECT_LAYOUT_ASSERT(RoadLaneCenterOffsetIs118,
                           offsetof(RoadSegmentLaneRuntime, center_position) ==
                           0x118);
TRACK_OBJECT_LAYOUT_ASSERT(RoadNegativeBoundOffsetIs12c,
                           offsetof(RoadSegmentLaneRuntime, road_left) ==
                           0x12C);
TRACK_OBJECT_LAYOUT_ASSERT(RoadPositiveBoundOffsetIs130,
                           offsetof(RoadSegmentLaneRuntime, road_right) ==
                           0x130);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionPositionOffsetIs1c,
                           offsetof(TrackInteractionObject, position) == 0x1C);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionNextOffsetIs00,
                           offsetof(TrackInteractionObject,
                                    list_node.links.next) == 0x00);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionPreviousOffsetIs04,
                           offsetof(TrackInteractionObject,
                                    list_node.links.previous) == 0x04);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionTrackOffsetIs2c,
                           offsetof(TrackInteractionObject, track_segment) ==
                           0x2C);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionCollisionOffsetIsc4,
                           offsetof(TrackInteractionObject,
                                    collision_flags) == 0xC4);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionMotionSourceOffsetIsec,
                           offsetof(TrackInteractionObject, motion_source) ==
                           0xEC);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionSelectorOffsetIsf0,
                           offsetof(TrackInteractionObject,
                                    interaction_selector) == 0xF0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackInteractionVelocityOffsetIs108,
                           offsetof(TrackInteractionObject, world_velocity) ==
                           0x108);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectAnimationOffsetIs70,
                           offsetof(TrackObject, animation) == 0x70);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectPreviousAnimationOffsetIs8c,
                           offsetof(TrackObject, previous_animation) == 0x8C);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectStatusOffsetIsc0,
                           offsetof(TrackObject, status_flags) == 0xC0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectCollisionOffsetIsc4,
                           offsetof(TrackObject, collision_flags) == 0xC4);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectVelocityOffsetIsec,
                           offsetof(TrackObject, velocity) == 0xEC);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectOrientationOffsetIs58,
                           offsetof(TrackObject, orientation) == 0x58);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectMovementHeadingOffsetIs5c,
                           offsetof(TrackObject,
                                    orientation.movement_heading) == 0x5C);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectHeadingOffsetIs128,
                           offsetof(TrackObject, heading) == 0x128);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectSnapshotOffsetIs44,
                           offsetof(TrackObject, position_snapshot) ==
                           0x44);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectPrepareUpdateOffsetIse0,
                           offsetof(TrackObject, prepare_update) == 0xE0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectRenderOffsetIsdc,
                           offsetof(TrackObject, render_object) == 0xDC);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectPerformUpdateOffsetIse4,
                           offsetof(TrackObject, perform_update) == 0xE4);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectResetOffsetIse8,
                           offsetof(TrackObject, reset_object) == 0xE8);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectWorldVelocityOffsetIs108,
                           offsetof(TrackObject, world_velocity) == 0x108);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectPrimaryTrackOffsetIs140,
                           offsetof(TrackObject, primary_track) == 0x140);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectCollisionWidthOffsetIs174,
                           offsetof(TrackObject, collision_half_width) ==
                           0x174);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectCollisionLengthOffsetIs178,
                           offsetof(TrackObject, collision_half_length) ==
                           0x178);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectContactOffsetIs1a0,
                           offsetof(TrackObject, contact) == 0x1A0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectCruiseSpeedOffsetIs1c0,
                           offsetof(TrackObject, cruise_speed) == 0x1C0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectModeOffsetIs1c8,
                           offsetof(TrackObject, mode) == 0x1C8);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectInheritedVelocityOffsetIs1cc,
                           offsetof(TrackObject, inherited_velocity) ==
                           0x1CC);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectMotionCommandOffsetIs1d8,
                           offsetof(TrackObject, motion_command) == 0x1D8);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectLaneOffsetIs1e4,
                           offsetof(TrackObject, lane) == 0x1E4);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectTargetOffsetIs1e8,
                           offsetof(TrackObject, target) == 0x1E8);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectNearbyOffsetIs1ec,
                           offsetof(TrackObject, nearby_object) == 0x1EC);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectModeUpdateOffsetIs1f0,
                           offsetof(TrackObject, mode_update_seen) == 0x1F0);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectDelayOffsetIs1f4,
                           offsetof(TrackObject, delay_ticks) == 0x1F4);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectSizeIs1f8,
                           sizeof(TrackObject) == 0x1F8);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectModeInitializationSizeIs18,
                           sizeof(TrackObjectModeInitialization) == 0x18);
TRACK_OBJECT_LAYOUT_ASSERT(TrackObjectModeTemplateSizesMatch,
                           sizeof(TrackObjectModeInitialization) ==
                           sizeof(TrackObjectModeTemplate));

#undef TRACK_OBJECT_LAYOUT_ASSERT

void
create_track_object(int                     mode,
                    int                     lane,
                    int                     offset,
                    TrackInteractionObject *parent,
                    RoadSegmentLaneRuntime *track);
int
approach_track_object_speed(const TrackObjectPosition *current_position,
                            int                        current_speed,
                            int                        maximum_speed,
                            int                        minimum_speed,
                            const TrackObjectPosition *target_position,
                            int                        target_speed,
                            int                        maximum_adjustment,
                            int                        distance_limit,
                            int                        target_offset,
                            int                        mode);
int
track_object_remains_active(TrackObject *object);
void
track_object_update(TrackObject *object);
void
track_object_mode_update(RacerEntity *racer);
int
track_object_draw(TrackObject *object,
                  int          bitmap);
int *
apply_track_object_steering(TrackObject              *object,
                            const TrackMotionCommand *command,
                            int                       maximum_forward_velocity,
                            int                       acceleration);

#endif
