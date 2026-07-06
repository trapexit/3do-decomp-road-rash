#ifndef RACER_RUNTIME_H
#define RACER_RUNTIME_H

#include "intrusive_list.h"
#include "rw_semantic_data.h"

#define RACER_LIST_NODE_OFFSET             0x000
#define RACER_VELOCITY_SIZE              0x0C
#define RIDER_ANIMATION_RUNTIME_SIZE     0x1C
#define RACER_STATUS_FLAGS_OFFSET        0xC0
#define RACER_SPATIAL_PITCH_SCALE_OFFSET 0xC8
#define RACER_SPATIAL_GAIN_SCALE_OFFSET  0xCC
#define RACER_RENDER_HEADING_OFFSET      0xD0
#define RACER_SPATIAL_PAN_OFFSET         0xD4
#define RACER_SPATIAL_SIDE_OFFSET        0xD8
#define RACER_VELOCITY_OFFSET            0xEC
#define RACER_COLLISION_EXTENTS_OFFSET   0x174
#define RACER_CONTACT_IMPULSE_OFFSET     0x184
#define RACER_OWNER_OFFSET               0x1C0
#define RACER_RUNTIME_SLOT_OFFSET        0x1D8
#define RACER_RACE_RANK_OFFSET           0x1F0
#define RACER_STEERING_ANGLE_OFFSET      0x284
#define RACER_ENGINE_PITCH_OFFSET        0x2D8
#define RACER_ENGINE_GAIN_OFFSET         0x2DC
#define RACER_ENGINE_RESERVED_OFFSET     0x2E0
#define RACER_PREVIOUS_ENGINE_VELOCITY_OFFSET 0x2E4
#define RACER_MAXIMUM_BIKE_HEALTH_OFFSET 0x2E8
#define RACER_CURRENT_BIKE_HEALTH_OFFSET 0x2EC
#define RACER_MODE_OFFSET                0x2F0
#define RACER_PAIRED_STATE_OFFSET        0x2F8
#define RACER_ENTITY_LAYOUT_SIZE         0x2FC

typedef struct RacerVelocity
{
  int forward;
  int lateral;
  int vertical;
} RacerVelocity;

typedef struct RacerOrientation
{
  int base_heading;
  int movement_heading;
  int steering_heading;
} RacerOrientation;

typedef struct RacerPositionSnapshot
{
  int position_z;
  int position_y;
  int position_aux;
  int position_x;
  struct RoadSegmentLaneRuntime *track_segment;
} RacerPositionSnapshot;

typedef enum RacerRole
{
  RACER_ROLE_PLAYER = 0,
  RACER_ROLE_OPPONENT = 1,
  RACER_ROLE_CHALLENGE = 2
} RacerRole;

enum RaceObjectHeadingValue
{
  RACE_OBJECT_ANGLE_HALF_TURN = 0x800000,
  RACE_OBJECT_ANGLE_FULL_TURN = 0x1000000
};

typedef struct RiderAnimationRuntime
{
  void *cans_resource;
  void *animation_resource;
  CelAnimationBinding **state_table;
  CelAnimationBinding *current_frame;
  unsigned char primary_state;
  unsigned char primary_frame;
  unsigned char secondary_state;
  unsigned char state_alignment;
  int next_frame_tick;
  signed char cycle_count;
  unsigned char cycle_alignment[3];
} RiderAnimationRuntime;

typedef struct TrackContactDescriptor
{
  unsigned char animation_state;
  unsigned char reserved_01;
  unsigned char response_kind;
  unsigned char reserved_03;
} TrackContactDescriptor;

typedef struct RacerEntity RacerEntity;
typedef void (*RaceObjectRenderCallback)(RacerEntity *object,
                                         void        *render_context);
typedef void (*RaceObjectUpdateCallback)(RacerEntity *object);
typedef void (*RaceObjectResetCallback)(RacerEntity *object,
                                        int          reason);
struct TrackInteractionObject;
enum RiderInteractionLayoutCount
{
  RIDER_INTERACTION_METRIC_COUNT = 10,
  RIDER_INTERACTION_METRIC_COMPONENT_COUNT = 3,
  RIDER_INTERACTION_RESPONSE_COUNT = 6,
  RIDER_PROFILE_LEVEL_COUNT = 5
};

typedef struct RiderInteractionMetric
{
  int components[RIDER_INTERACTION_METRIC_COMPONENT_COUNT];
} RiderInteractionMetric;

typedef struct RiderInteractionRuntime
{
  RiderInteractionMetric current_metrics[RIDER_INTERACTION_METRIC_COUNT];
  RiderInteractionMetric target_metrics[RIDER_INTERACTION_METRIC_COUNT];
  RiderInteractionMetric response_deltas[RIDER_INTERACTION_RESPONSE_COUNT];
  int handler_elapsed[RIDER_INTERACTION_METRIC_COUNT];
  int handler_index;
  unsigned char handler_phase;
  signed char metric_index;
  unsigned char handler_alignment[2];
  RacerEntity *attack_target;
  RacerEntity *left_candidate;
  RacerEntity *right_candidate;
  int attack_target_class;
  int left_candidate_class;
  int right_candidate_class;
  struct TrackInteractionObject *avoidance_target;
  unsigned char reserved_184[0x04];
  int pacing_frame_accumulator;
  signed char lane_selection_mode;
  unsigned char lane_selection_alignment[3];
  int lane_center_bias;
  int target_lateral_position;
  int target_track_position;
  int target_speed;
  int target_acceleration;
  int target_speed_limit;
  int profile_values[RIDER_PROFILE_LEVEL_COUNT];
  int pacing_values[RIDER_PROFILE_LEVEL_COUNT];
  signed char attack_kick_roll_divisor;
  signed char attack_alternate_roll_divisor;
  unsigned char attack_roll_alignment[2];
  int engagement_radius;
} RiderInteractionRuntime;
struct RoadSegmentLaneRuntime;

struct RacerEntity
{
  PooledListNode list_node;
  int next_update_tick;
  int last_update_tick;
  int update_interval_ticks;
  int time_step;
  int position_z;
  int position_y;
  int position_aux;
  int position_x;
  struct RoadSegmentLaneRuntime *track_segment;
  int previous_position_z;
  int previous_position_y;
  int previous_position_aux;
  int previous_position_x;
  struct RoadSegmentLaneRuntime *previous_track_segment;
  RacerPositionSnapshot position_snapshot;
  RacerOrientation orientation;
  RacerOrientation previous_orientation;
  RiderAnimationRuntime animation;
  RiderAnimationRuntime previous_animation;
  int reserved_0a8;
  void *animation_state_target;
  int reserved_0b0;
  int reserved_0b4;
  int reserved_0b8;
  int render_position_z;
  unsigned char status_flags;
  unsigned char status_alignment[3];
  int collision_class;
  int spatial_pitch_scale_8_8;
  int spatial_gain_scale_8_8;
  int render_heading;
  int spatial_pan_offset;
  int spatial_longitudinal_side;
  RaceObjectRenderCallback render_object;
  RaceObjectUpdateCallback prepare_update;
  RaceObjectUpdateCallback perform_update;
  RaceObjectResetCallback reset_object;
  RacerVelocity velocity;
  int reserved_0f8;
  int reserved_0fc;
  int reserved_100;
  int lateral_velocity_sample;
  RacerVelocity world_velocity;
  int drive_acceleration;
  int steering_force;
  int surface_acceleration_min;
  int surface_acceleration_max;
  TrackContactDescriptor *track_contact;
  int heading;
  int left_surface_bound;
  int right_surface_bound;
  int left_collision_bound;
  int right_collision_bound;
  struct RoadSegmentLaneRuntime *secondary_track;
  struct RoadSegmentLaneRuntime *primary_track;
  RacerEntity *collision_peer;
  int base_slip_budget;
  int active_slip_budget;
  int reserved_150;
  int base_drag;
  int surface_drag;
  int drag_limit;
  int surface_contact_scale_8_8;
  unsigned char slip_amount;
  unsigned char surface_contact_alignment[3];
  int minimum_acceleration;
  int contact_threshold;
  int bounce_scale;
  int collision_half_width_x;
  int collision_half_length_z;
  int collision_half_height_y;
  int collision_impulse_scale_8_8;
  RacerVelocity contact_impulse;
  int other_collision_mass;
  int collision_mass;
  int collision_scratch;
  unsigned char collision_flags;
  unsigned char collision_alignment[3];
  RacerEntity *contact;
  RacerContactCallback contact_callback;
  int template_enabled;
  int template_visible;
  int impact_cooldown;
  int impact_strength;
  int impact_scale;
  unsigned char collision_profile_id;
  unsigned char collision_profile_alignment[3];
  RacerEntity *owner;
  int queued_attack_style;
  int attack_target_refresh_tick;
  int attack_contact_tick;
  int attack_style;
  RiderInteractionRuntime *interaction_data;
  RiderRuntimeSlot *runtime;
  unsigned char pending_contact_animation;
  unsigned char pending_contact_animation_frame;
  unsigned char contact_animation_alignment[2];
  int last_contact_track_cell;
  int contact_frame_tick;
  unsigned char reserved_1e8[0x08];
  int race_rank;
  unsigned char reserved_1f4[0x4C];
  unsigned char acceleration_disabled;
  unsigned char acceleration_alignment[3];
  int base_engine_pitch;
  int idle_engine_pitch;
  int forward_speed_factor;
  int forward_acceleration_divisor;
  int forward_deceleration_divisor;
  int steering_speed_scale;
  int acceleration_limit;
  int acceleration_rise_step;
  int acceleration_fall_step;
  int acceleration_curve_a;
  int acceleration_curve_b;
  int acceleration_curve_c;
  unsigned char action_flags;
  unsigned char action_alignment[3];
  int braking_target;
  int braking_rise_step;
  int braking_fall_step;
  int steering_angle;
  int previous_steering_angle;
  int slide_activation_threshold;
  int slide_grip_scale_8_8;
  int slide_timer;
  int steering_limit;
  int primary_steering_response;
  int secondary_steering_response;
  int primary_steering_response_step;
  int secondary_steering_response_step;
  unsigned char reserved_2ac[0x08];
  int surface_heading_scale_8_8;
  int alternate_surface_heading_scale_8_8;
  int reserved_2bc;
  int engine_response_interval;
  unsigned char reserved_2c4[0x10];
  int collision_steering_gain_8_8;
  int engine_pitch;
  int engine_gain;
  int reserved_2e0;
  int previous_engine_forward_velocity_target;
  int maximum_bike_health;
  int current_bike_health;
  int rider_mode;
  int recovery_kind;
  RacerEntity *paired_state;
};

typedef char RacerVelocitySizeCheck[
    (sizeof(RacerVelocity) == RACER_VELOCITY_SIZE) ? 1 : -1];
typedef char RiderAnimationRuntimeSizeCheck[
    (sizeof(RiderAnimationRuntime) == RIDER_ANIMATION_RUNTIME_SIZE) ? 1 : -1];
typedef char RiderAnimationCansResourceOffsetCheck[
    (offsetof(RiderAnimationRuntime, cans_resource) == 0x00) ? 1 : -1];
typedef char RiderAnimationResourceOffsetCheck[
    (offsetof(RiderAnimationRuntime, animation_resource) == 0x04) ? 1 : -1];
typedef char RiderAnimationStateTableOffsetCheck[
    (offsetof(RiderAnimationRuntime, state_table) == 0x08) ? 1 : -1];
typedef char RiderAnimationCurrentFrameOffsetCheck[
    (offsetof(RiderAnimationRuntime, current_frame) == 0x0C) ? 1 : -1];
typedef char RiderAnimationPrimaryStateOffsetCheck[
    (offsetof(RiderAnimationRuntime, primary_state) == 0x10) ? 1 : -1];
typedef char RiderAnimationNextFrameTickOffsetCheck[
    (offsetof(RiderAnimationRuntime, next_frame_tick) == 0x14) ? 1 : -1];
typedef char RiderAnimationCycleCountOffsetCheck[
    (offsetof(RiderAnimationRuntime, cycle_count) == 0x18) ? 1 : -1];
typedef char RacerListNodeOffsetCheck[
    (offsetof(RacerEntity, list_node) == RACER_LIST_NODE_OFFSET) ? 1 : -1];
typedef char RacerListNextOffsetCheck[
    (offsetof(RacerEntity, list_node) + offsetof(PooledListNode, links) +
     offsetof(IntrusiveListNode, next) == 0x00) ? 1 : -1];
typedef char RacerListPreviousOffsetCheck[
    (offsetof(RacerEntity, list_node) + offsetof(PooledListNode, links) +
     offsetof(IntrusiveListNode, previous) == 0x04) ? 1 : -1];
typedef char RacerListOwnerOffsetCheck[
    (offsetof(RacerEntity, list_node) + offsetof(PooledListNode, owner_pool) ==
     0x08) ? 1 : -1];
typedef char RacerStatusFlagsOffsetCheck[
    (offsetof(RacerEntity, status_flags) == RACER_STATUS_FLAGS_OFFSET)
        ? 1 : -1];
typedef char RacerUpdateIntervalOffsetCheck[
    (offsetof(RacerEntity, update_interval_ticks) == 0x14) ? 1 : -1];
typedef char RacerPrepareUpdateOffsetCheck[
    (offsetof(RacerEntity, prepare_update) == 0xE0) ? 1 : -1];
typedef char RacerRenderObjectOffsetCheck[
    (offsetof(RacerEntity, render_object) == 0xDC) ? 1 : -1];
typedef char RacerPerformUpdateOffsetCheck[
    (offsetof(RacerEntity, perform_update) == 0xE4) ? 1 : -1];
typedef char RacerResetObjectOffsetCheck[
    (offsetof(RacerEntity, reset_object) == 0xE8) ? 1 : -1];
typedef char RacerAnimationStateTargetOffsetCheck[
    (offsetof(RacerEntity, animation_state_target) == 0xAC) ? 1 : -1];
typedef char RacerPreviousAnimationOffsetCheck[
    (offsetof(RacerEntity, previous_animation) == 0x8C) ? 1 : -1];
typedef char RacerPositionSnapshotOffsetCheck[
    (offsetof(RacerEntity, position_snapshot) == 0x44) ? 1 : -1];
typedef char RacerPreviousOrientationOffsetCheck[
    (offsetof(RacerEntity, previous_orientation) == 0x64) ? 1 : -1];
typedef char RacerSpatialPitchScaleOffsetCheck[
    (offsetof(RacerEntity, spatial_pitch_scale_8_8) ==
     RACER_SPATIAL_PITCH_SCALE_OFFSET) ? 1 : -1];
typedef char RacerSpatialGainScaleOffsetCheck[
    (offsetof(RacerEntity, spatial_gain_scale_8_8) ==
     RACER_SPATIAL_GAIN_SCALE_OFFSET) ? 1 : -1];
typedef char RacerRenderHeadingOffsetCheck[
    (offsetof(RacerEntity, render_heading) == RACER_RENDER_HEADING_OFFSET)
        ? 1 : -1];
typedef char RacerSpatialPanOffsetCheck[
    (offsetof(RacerEntity, spatial_pan_offset) == RACER_SPATIAL_PAN_OFFSET)
        ? 1 : -1];
typedef char RacerSpatialSideOffsetCheck[
    (offsetof(RacerEntity, spatial_longitudinal_side) ==
     RACER_SPATIAL_SIDE_OFFSET) ? 1 : -1];
typedef char RacerVelocityOffsetCheck[
    (offsetof(RacerEntity, velocity) == RACER_VELOCITY_OFFSET) ? 1 : -1];
typedef char RacerMinimumAccelerationOffsetCheck[
    (offsetof(RacerEntity, minimum_acceleration) == 0x168) ? 1 : -1];
typedef char RacerContactThresholdOffsetCheck[
    (offsetof(RacerEntity, contact_threshold) == 0x16C) ? 1 : -1];
typedef char RacerBounceScaleOffsetCheck[
    (offsetof(RacerEntity, bounce_scale) == 0x170) ? 1 : -1];
typedef char RacerCollisionExtentsOffsetCheck[
    (offsetof(RacerEntity, collision_half_width_x) ==
     RACER_COLLISION_EXTENTS_OFFSET) ? 1 : -1];
typedef char RacerCollisionPeerOffsetCheck[
    (offsetof(RacerEntity, collision_peer) == 0x144) ? 1 : -1];
typedef char RacerContactImpulseOffsetCheck[
    (offsetof(RacerEntity, contact_impulse) ==
     RACER_CONTACT_IMPULSE_OFFSET) ? 1 : -1];
typedef char RacerTemplateEnabledOffsetCheck[
    (offsetof(RacerEntity, template_enabled) == 0x1A8) ? 1 : -1];
typedef char RacerTemplateVisibleOffsetCheck[
    (offsetof(RacerEntity, template_visible) == 0x1AC) ? 1 : -1];
typedef char RacerImpactCooldownOffsetCheck[
    (offsetof(RacerEntity, impact_cooldown) == 0x1B0) ? 1 : -1];
typedef char RacerImpactStrengthOffsetCheck[
    (offsetof(RacerEntity, impact_strength) == 0x1B4) ? 1 : -1];
typedef char RacerImpactScaleOffsetCheck[
    (offsetof(RacerEntity, impact_scale) == 0x1B8) ? 1 : -1];
typedef char RacerCollisionProfileOffsetCheck[
    (offsetof(RacerEntity, collision_profile_id) == 0x1BC) ? 1 : -1];
typedef char RacerOwnerOffsetCheck[
    (offsetof(RacerEntity, owner) == RACER_OWNER_OFFSET) ? 1 : -1];
typedef char RacerRuntimeSlotOffsetCheck[
    (offsetof(RacerEntity, runtime) == RACER_RUNTIME_SLOT_OFFSET) ? 1 : -1];
typedef char RiderInteractionAttackTargetOffsetCheck[
    (offsetof(RiderInteractionRuntime, attack_target) == 0x168)
        ? 1 : -1];
typedef char RiderInteractionCurrentMetricsOffsetCheck[
    (offsetof(RiderInteractionRuntime, current_metrics) == 0x000)
        ? 1 : -1];
typedef char RiderInteractionTargetMetricsOffsetCheck[
    (offsetof(RiderInteractionRuntime, target_metrics) == 0x078)
        ? 1 : -1];
typedef char RiderInteractionResponseDeltasOffsetCheck[
    (offsetof(RiderInteractionRuntime, response_deltas) == 0x0F0)
        ? 1 : -1];
typedef char RiderInteractionHandlerElapsedOffsetCheck[
    (offsetof(RiderInteractionRuntime, handler_elapsed) == 0x138)
        ? 1 : -1];
typedef char RiderInteractionHandlerIndexOffsetCheck[
    (offsetof(RiderInteractionRuntime, handler_index) == 0x160)
        ? 1 : -1];
typedef char RiderInteractionHandlerPhaseOffsetCheck[
    (offsetof(RiderInteractionRuntime, handler_phase) == 0x164)
        ? 1 : -1];
typedef char RiderInteractionMetricIndexOffsetCheck[
    (offsetof(RiderInteractionRuntime, metric_index) == 0x165)
        ? 1 : -1];
typedef char RiderInteractionLeftCandidateOffsetCheck[
    (offsetof(RiderInteractionRuntime, left_candidate) == 0x16C)
        ? 1 : -1];
typedef char RiderInteractionRightCandidateOffsetCheck[
    (offsetof(RiderInteractionRuntime, right_candidate) == 0x170)
        ? 1 : -1];
typedef char RiderInteractionAttackClassOffsetCheck[
    (offsetof(RiderInteractionRuntime, attack_target_class) == 0x174)
        ? 1 : -1];
typedef char RiderInteractionAvoidanceTargetOffsetCheck[
    (offsetof(RiderInteractionRuntime, avoidance_target) == 0x180)
        ? 1 : -1];
typedef char RiderInteractionPacingFrameOffsetCheck[
    (offsetof(RiderInteractionRuntime, pacing_frame_accumulator) == 0x188)
        ? 1 : -1];
typedef char RiderInteractionLaneModeOffsetCheck[
    (offsetof(RiderInteractionRuntime, lane_selection_mode) == 0x18C)
        ? 1 : -1];
typedef char RiderInteractionLaneBiasOffsetCheck[
    (offsetof(RiderInteractionRuntime, lane_center_bias) == 0x190)
        ? 1 : -1];
typedef char RiderInteractionLateralTargetOffsetCheck[
    (offsetof(RiderInteractionRuntime, target_lateral_position) == 0x194)
        ? 1 : -1];
typedef char RiderInteractionTrackTargetOffsetCheck[
    (offsetof(RiderInteractionRuntime, target_track_position) == 0x198)
        ? 1 : -1];
typedef char RiderInteractionSpeedTargetOffsetCheck[
    (offsetof(RiderInteractionRuntime, target_speed) == 0x19C)
        ? 1 : -1];
typedef char RiderInteractionSpeedLimitOffsetCheck[
    (offsetof(RiderInteractionRuntime, target_speed_limit) == 0x1A4)
        ? 1 : -1];
typedef char RiderInteractionProfileValuesOffsetCheck[
    (offsetof(RiderInteractionRuntime, profile_values) == 0x1A8)
        ? 1 : -1];
typedef char RiderInteractionPacingValuesOffsetCheck[
    (offsetof(RiderInteractionRuntime, pacing_values) == 0x1BC)
        ? 1 : -1];
typedef char RiderInteractionKickRollOffsetCheck[
    (offsetof(RiderInteractionRuntime, attack_kick_roll_divisor) == 0x1D0)
        ? 1 : -1];
typedef char RiderInteractionEngagementRadiusOffsetCheck[
    (offsetof(RiderInteractionRuntime, engagement_radius) == 0x1D4)
        ? 1 : -1];
typedef char RacerRaceRankOffsetCheck[
    (offsetof(RacerEntity, race_rank) == RACER_RACE_RANK_OFFSET) ? 1 : -1];
typedef char RacerActionFlagsOffsetCheck[
    (offsetof(RacerEntity, action_flags) == 0x274) ? 1 : -1];
typedef char RacerSteeringAngleOffsetCheck[
    (offsetof(RacerEntity, steering_angle) == RACER_STEERING_ANGLE_OFFSET)
        ? 1 : -1];
typedef char RacerSteeringSpeedScaleOffsetCheck[
    (offsetof(RacerEntity, steering_speed_scale) == 0x258) ? 1 : -1];
typedef char RacerBaseSlipBudgetOffsetCheck[
    (offsetof(RacerEntity, base_slip_budget) == 0x148) ? 1 : -1];
typedef char RacerActiveSlipBudgetOffsetCheck[
    (offsetof(RacerEntity, active_slip_budget) == 0x14C) ? 1 : -1];
typedef char RacerPreviousSteeringAngleOffsetCheck[
    (offsetof(RacerEntity, previous_steering_angle) == 0x288) ? 1 : -1];
typedef char RacerSlideActivationThresholdOffsetCheck[
    (offsetof(RacerEntity, slide_activation_threshold) == 0x28C) ? 1 : -1];
typedef char RacerSlideGripScaleOffsetCheck[
    (offsetof(RacerEntity, slide_grip_scale_8_8) == 0x290) ? 1 : -1];
typedef char RacerSlideTimerOffsetCheck[
    (offsetof(RacerEntity, slide_timer) == 0x294) ? 1 : -1];
typedef char RacerPrimarySteeringResponseOffsetCheck[
    (offsetof(RacerEntity, primary_steering_response) == 0x29C)
        ? 1 : -1];
typedef char RacerSecondarySteeringResponseOffsetCheck[
    (offsetof(RacerEntity, secondary_steering_response) == 0x2A0)
        ? 1 : -1];
typedef char RacerPrimarySteeringStepOffsetCheck[
    (offsetof(RacerEntity, primary_steering_response_step) == 0x2A4)
        ? 1 : -1];
typedef char RacerSecondarySteeringStepOffsetCheck[
    (offsetof(RacerEntity, secondary_steering_response_step) == 0x2A8)
        ? 1 : -1];
typedef char RacerSurfaceHeadingScaleOffsetCheck[
    (offsetof(RacerEntity, surface_heading_scale_8_8) == 0x2B4)
        ? 1 : -1];
typedef char RacerAlternateSurfaceHeadingScaleOffsetCheck[
    (offsetof(RacerEntity, alternate_surface_heading_scale_8_8) == 0x2B8)
        ? 1 : -1];
typedef char RacerEngineResponseIntervalOffsetCheck[
    (offsetof(RacerEntity, engine_response_interval) == 0x2C0)
        ? 1 : -1];
typedef char RacerEnginePitchOffsetCheck[
    (offsetof(RacerEntity, engine_pitch) == RACER_ENGINE_PITCH_OFFSET)
        ? 1 : -1];
typedef char RacerEngineGainOffsetCheck[
    (offsetof(RacerEntity, engine_gain) == RACER_ENGINE_GAIN_OFFSET)
        ? 1 : -1];
typedef char RacerEngineReservedOffsetCheck[
    (offsetof(RacerEntity, reserved_2e0) == RACER_ENGINE_RESERVED_OFFSET)
        ? 1 : -1];
typedef char RacerPreviousEngineVelocityOffsetCheck[
    (offsetof(RacerEntity, previous_engine_forward_velocity_target) ==
     RACER_PREVIOUS_ENGINE_VELOCITY_OFFSET) ? 1 : -1];
typedef char RacerMaximumBikeHealthOffsetCheck[
    (offsetof(RacerEntity, maximum_bike_health) ==
     RACER_MAXIMUM_BIKE_HEALTH_OFFSET) ? 1 : -1];
typedef char RacerCurrentBikeHealthOffsetCheck[
    (offsetof(RacerEntity, current_bike_health) ==
     RACER_CURRENT_BIKE_HEALTH_OFFSET) ? 1 : -1];
typedef char RacerModeOffsetCheck[
    (offsetof(RacerEntity, rider_mode) == RACER_MODE_OFFSET) ? 1 : -1];
typedef char RacerPairedStateOffsetCheck[
    (offsetof(RacerEntity, paired_state) == RACER_PAIRED_STATE_OFFSET)
        ? 1 : -1];
typedef char RacerEntitySizeCheck[
    (sizeof(RacerEntity) == RACER_ENTITY_LAYOUT_SIZE) ? 1 : -1];

int
classify_rider_surface_zone(char *lane_,
                            int   fixed_position_);
// Classifies even airborne/recovering riders; traction preserves the classifier inputs.
int
update_rider_traction_and_heading(RacerEntity *rider_);
// surface_zone_ is the current lane/position classification, ignored without surface contact.
void
update_racer_forward_velocity(RacerEntity *racer_,
                              int          surface_zone_);

void
reset_rider_to_normal_mode(RacerEntity *rider);
void
sync_rider_state_from_object(RacerEntity *rider);
void *
create_racer_entity_from_template(void                          *object_pool,
                                  const RacerCreationTemplate   *creation_template,
                                  int                            lateral_position,
                                  int                            track_offset,
                                  struct RoadSegmentLaneRuntime *track_segment);
void
begin_rider_recovery(RacerEntity         *rider,
                     const RacerVelocity *state_velocity_delta,
                     const RacerVelocity *rider_velocity_delta,
                     int                  recovery_kind);
int
apply_rider_health_damage(RacerEntity *rider,
                          int          damage);
int
apply_bike_health_damage(RacerEntity *rider,
                         int          damage);
int
apply_rider_collision_damage(RacerEntity *rider,
                             int          rider_damage,
                             int          bike_damage,
                             int          recovery_kind);
void
racer_state_contact_response(RacerEntity *state);
void
racer_object_contact_response(RacerEntity *rider);
void
resolve_racer_entity_collision(RacerEntity *first,
                               RacerEntity *second);
int
set_rider_longitudinal_control_mode(RacerEntity *rider,
                                    int          mode);
int
set_rider_steering_control_mode(RacerEntity *rider,
                                int          mode,
                                int          turn_direction);
int
approach_rider_acceleration_target(RacerEntity *rider,
                                   int          target);
int
approach_rider_braking_target(RacerEntity *rider,
                              int          target);
int
approach_rider_steering_target(RacerEntity *rider,
                               int          target_angle,
                               int          turn_direction);
int *
apply_race_object_heading_and_force(RacerEntity *object,
                                    int          heading,
                                    int          maximum_forward_velocity,
                                    int          acceleration);

#endif
