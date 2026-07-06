#ifndef ROADRASH_RIDER_VISUAL_RUNTIME_H
#define ROADRASH_RIDER_VISUAL_RUNTIME_H

#include "cans_animation_binding.h"
#include "track_object_runtime.h"
#include "Types.h"

#ifndef offsetof
  #include "stddef.h"
#endif

enum RiderVisualAnimationState
{
  RIDER_ANIMATION_MOTION = 0,
  RIDER_ANIMATION_STANDING = 1,
  RIDER_ANIMATION_SHAKE = 2,
  RIDER_ANIMATION_FALL = 3,
  RIDER_ANIMATION_ATTACK = 4,
  RIDER_ANIMATION_FAST = 5,
  RIDER_ANIMATION_FLAG = 6,
  RIDER_ANIMATION_STATE_COUNT = 7,
  RIDER_ANIMATION_NONE = -1
};

enum RiderVisualBehaviorState
{
  RIDER_BEHAVIOR_WAITING = 0,
  RIDER_BEHAVIOR_RIDING = 1,
  RIDER_BEHAVIOR_CONTACT_LOCKED = 2,
  RIDER_BEHAVIOR_RECOVERING = 3,
  RIDER_BEHAVIOR_SHAKE_RECOVERY = 4,
  RIDER_BEHAVIOR_FAST_RECOVERY = 5,
  RIDER_BEHAVIOR_FLAG = 7
};

enum RiderVisualTravelMode
{
  RIDER_TRAVEL_FIXED_HEADING = 4,
  RIDER_TRAVEL_HARD_LEFT = 5,
  RIDER_TRAVEL_CENTER = 6,
  RIDER_TRAVEL_HARD_RIGHT = 7,
  RIDER_TRAVEL_FLAG = 10,
  RIDER_TRAVEL_MODE_COUNT = 11
};

enum RiderVisualRuntimeConstant
{
  RIDER_VISUAL_POOL_CAPACITY = 15,
  RIDER_EFFECT_NEGATIVE_SIDE_TYPE = 2,
  RIDER_EFFECT_SEARCH_DISTANCE = 0x1E0,
  RIDER_EFFECT_CONTACT_DISTANCE = 0x800,
  RIDER_EFFECT_REQUIRED_SURFACE_FLAGS = 0x18,
  RIDER_EFFECT_NOT_FOUND = -1,
  RIDER_EFFECT_TARGET_SIDE = 2,
  RIDER_DIRECTION_HARD_LEFT = 0xFF,
  RIDER_DIRECTION_CENTER = 0x80,
  RIDER_DIRECTION_HARD_RIGHT = 1,
  RIDER_DIRECTION_PROBE_SHIFT = 2,
  RIDER_DIRECTION_PROBE_BASE = 0x10,
  RIDER_LANE_ROTATION_STEP = 2,
  RIDER_LANE_ROTATION_MASK = 3,
  RIDER_ACTION_FORCE_LOW = 4,
  RIDER_ACTION_FORCE_HIGH = 8,
  RIDER_ACTION_FORCE_SCALE = 100,
  RIDER_ACTION_COOLDOWN_TICKS = 0xA0,
  RIDER_SOUND_COOLDOWN_TICKS = 0xB4,
  RIDER_RANDOM_ODD_MASK = 1,
  RIDER_RANDOM_ACTION_MASK = 3,
  RIDER_RANDOM_FAST_MASK = 7,
  RIDER_FAMILY_ITEM_TYPE = 2,
  RIDER_FAMILY_ANIMATION_SLOT = 0,
  RIDER_FAMILY_CANS_SLOT = 1,
  RIDER_FAMILY_CONTACT_SAMPLE_SLOT = 2,
  RIDER_FAMILY_ATTACK_SAMPLE_SLOT = 3,
  RIDER_CHALLENGE_LANE_RANDOM_DIVISOR = 0x81,
  RIDER_CHALLENGE_LANE_BASE_OFFSET = 0x2000,
  RIDER_CHALLENGE_LANE_RANDOM_SHIFT = 4,
  RIDER_EFFECT_COOLDOWN_RANDOM_DIVISOR = 0x4FB1
};

enum RiderVisualContactConstant
{
  RIDER_CONTACT_ATTACK_CLASS = 4,
  RIDER_CONTACT_RACER_CLASS = 8,
  RIDER_CONTACT_RECOVERY_MODE = 2,
  RIDER_CONTACT_MINIMUM_SPEED = 0x3E8,
  RIDER_CONTACT_SOUND_WINDOW = 0x400,
  RIDER_CONTACT_STOP_FORCE = 0x4E20,
  RIDER_CONTACT_DELAY_TICKS = 0x19,
  RIDER_REVERSE_MOVEMENT_HEADING = 0x7F,
  RIDER_TRACK_SNAP_DISTANCE = 0x200,
  RIDER_TRACK_SNAP_BIAS = 0x1FF,
  RIDER_RENDER_PROJECT_SCALE = 0x200,
  RIDER_RENDER_POSITION_SHIFT = 8,
  RIDER_DIVIDE_BY_SIXTEEN_SHIFT = 4,
  RIDER_DIVIDE_BY_SIXTEEN_BIAS = 0x0F,
  RIDER_DIVIDE_BY_256_SHIFT = 8,
  RIDER_DIVIDE_BY_256_BIAS = 0xFF
};

#define RIDER_FIXED_MOVEMENT_DIRECTION ((int)0xFF800800U)
#define RIDER_ANIMATION_RESOURCE_TAG 0x414E494DU

typedef struct RiderVisualObject
{
  unsigned char reserved_000[0x18];
  int time_step;
  TrackObjectPosition position;
  RoadSegmentLaneRuntime *track_segment;
  unsigned char reserved_030[0x14];
  int render_x;
  int render_y_fraction;
  int render_y;
  int render_z_fraction;
  unsigned char reserved_054[0x1C];
  RiderAnimationRuntime animation;
  AnimationResourcePair render_resources;
  unsigned char reserved_094[0x04];
  CelAnimationBinding *render_binding;
  unsigned char reserved_09c[0x0C];
  CCB *render_ccb;
  unsigned char reserved_0ac[0x04];
  int render_width;
  int render_height;
  int render_source_offset;
  int render_depth;
  unsigned char status_flags;
  unsigned char status_alignment[3];
  int collision_flags;
  unsigned char reserved_0c8[0x24];
  RacerVelocity velocity;
  unsigned char reserved_0f8[0x0C];
  int lateral_velocity_sample;
  RacerVelocity world_velocity;
  unsigned char reserved_114[0x14];
  int movement_heading;
  unsigned char reserved_12c[0x28];
  int contact_delay;
  unsigned char reserved_158[0x48];
  RacerEntity *contact;
  unsigned char reserved_1a4[0x1C];
  TrackObjectPosition target_position;
  RoadSegmentLaneRuntime *target_track_segment;
  int family_resource_inventory;
  unsigned char animation_ready;
  unsigned char animation_ready_alignment[3];
  int travel_mode;
  int behavior_state;
  int lane_direction_index;
  int movement_direction;
  int movement_force;
  int action_cooldown_tick;
  signed char observed_lane;
  unsigned char lane_probe_pending;
  unsigned char lane_alignment[2];
  unsigned int last_sound_tick;
  unsigned char reserved_1fc[0x04];
  CelAnimationBinding *animation_states[RIDER_ANIMATION_STATE_COUNT];
} RiderVisualObject;

typedef struct RiderVisualPoolSlot
{
  RiderVisualObject object;
  CelAnimationBinding motion_bindings[6];
  CelAnimationBinding standing_bindings[1];
  CelAnimationBinding shake_bindings[2];
  CelAnimationBinding fall_bindings[6];
  CelAnimationBinding attack_bindings[2];
  CelAnimationBinding fast_bindings[6];
  CelAnimationBinding flag_bindings[6];
} RiderVisualPoolSlot;

int
compute_track_lane_index(const RiderVisualObject *rider,
                         int                      probe_mode);
boolean
is_rider_lane_change_clear(const RiderVisualObject *rider,
                           int                      direction,
                           int                      lane_offset);

#define RIDER_VISUAL_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualTimeStepOffsetIs18,
                           offsetof(RiderVisualObject, time_step) == 0x18);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualPositionOffsetIs1c,
                           offsetof(RiderVisualObject, position) == 0x1C);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualTrackOffsetIs2c,
                           offsetof(RiderVisualObject, track_segment) == 0x2C);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualAnimationOffsetIs70,
                           offsetof(RiderVisualObject, animation) == 0x70);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualRenderBindingOffsetIs98,
                           offsetof(RiderVisualObject, render_binding) ==
                           0x98);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualRenderCcbOffsetIsa8,
                           offsetof(RiderVisualObject, render_ccb) == 0xA8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualRenderDepthOffsetIsbc,
                           offsetof(RiderVisualObject, render_depth) == 0xBC);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualCollisionOffsetIsc4,
                           offsetof(RiderVisualObject, collision_flags) == 0xC4);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualVelocityOffsetIsec,
                           offsetof(RiderVisualObject, velocity) == 0xEC);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualWorldVelocityOffsetIs108,
                           offsetof(RiderVisualObject, world_velocity) == 0x108);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualMovementHeadingOffsetIs128,
                           offsetof(RiderVisualObject, movement_heading) ==
                           0x128);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualContactOffsetIs1a0,
                           offsetof(RiderVisualObject, contact) == 0x1A0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualTargetPositionOffsetIs1c0,
                           offsetof(RiderVisualObject, target_position) ==
                           0x1C0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualTargetTrackOffsetIs1d0,
                           offsetof(RiderVisualObject,
                                    target_track_segment) == 0x1D0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualInventoryOffsetIs1d4,
                           offsetof(RiderVisualObject,
                                    family_resource_inventory) == 0x1D4);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualAnimationReadyOffsetIs1d8,
                           offsetof(RiderVisualObject, animation_ready) ==
                           0x1D8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualTravelModeOffsetIs1dc,
                           offsetof(RiderVisualObject, travel_mode) == 0x1DC);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualBehaviorOffsetIs1e0,
                           offsetof(RiderVisualObject, behavior_state) ==
                           0x1E0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualLaneDirectionOffsetIs1e4,
                           offsetof(RiderVisualObject,
                                    lane_direction_index) == 0x1E4);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualMovementDirectionOffsetIs1e8,
                           offsetof(RiderVisualObject, movement_direction) ==
                           0x1E8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualMovementForceOffsetIs1ec,
                           offsetof(RiderVisualObject, movement_force) ==
                           0x1EC);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualActionCooldownOffsetIs1f0,
                           offsetof(RiderVisualObject,
                                    action_cooldown_tick) == 0x1F0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualObservedLaneOffsetIs1f4,
                           offsetof(RiderVisualObject, observed_lane) ==
                           0x1F4);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualLastSoundOffsetIs1f8,
                           offsetof(RiderVisualObject, last_sound_tick) ==
                           0x1F8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualAnimationStatesOffsetIs200,
                           offsetof(RiderVisualObject, animation_states) ==
                           0x200);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualStateTableEndIs21c,
                           sizeof(RiderVisualObject) == 0x21C);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualMotionBindingsOffsetIs21c,
                           offsetof(RiderVisualPoolSlot, motion_bindings) ==
                           0x21C);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualStandingBindingsOffsetIs294,
                           offsetof(RiderVisualPoolSlot,
                                    standing_bindings) == 0x294);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualShakeBindingsOffsetIs2a8,
                           offsetof(RiderVisualPoolSlot, shake_bindings) ==
                           0x2A8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualFallBindingsOffsetIs2d0,
                           offsetof(RiderVisualPoolSlot, fall_bindings) ==
                           0x2D0);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualAttackBindingsOffsetIs348,
                           offsetof(RiderVisualPoolSlot, attack_bindings) ==
                           0x348);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualFastBindingsOffsetIs370,
                           offsetof(RiderVisualPoolSlot, fast_bindings) ==
                           0x370);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualFlagBindingsOffsetIs3e8,
                           offsetof(RiderVisualPoolSlot, flag_bindings) ==
                           0x3E8);
RIDER_VISUAL_LAYOUT_ASSERT(RiderVisualPoolSlotSizeIs460,
                           sizeof(RiderVisualPoolSlot) == 0x460);

#undef RIDER_VISUAL_LAYOUT_ASSERT

int
find_nearby_track_effect_side(RiderVisualObject *rider);
void
initialize_rider_visual_animations(RiderVisualObject *rider);
int
destroy_rider_visual_pool(void);
void
rider_visual_create(RiderVisualObject *rider,
                    int                bitmap);
void
rider_visual_contact(RacerEntity *object);
void
rider_visual_update(RiderVisualObject *rider);
void
rider_visual_destroy(RiderVisualObject *rider);

#endif
