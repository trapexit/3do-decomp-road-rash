#ifndef RIDER_PHYSICS_CONFIGURATION_H
#define RIDER_PHYSICS_CONFIGURATION_H

enum
{
  RIDER_GEAR_PROFILE_COUNT = 6,
  RIDER_GEAR_RUNTIME_PROFILE_COUNT = RIDER_GEAR_PROFILE_COUNT + 1,
  RIDER_PHYSICS_CONFIGURATION_SIZE = 0x2F0
};

typedef struct RiderGearProfile
{
  int engine_pitch;
  int maximum_forward_speed;
  int upshift_speed;
  int downshift_speed;
} RiderGearProfile;

/*
 * Serialized SPEC resource layout copied into the physics portion of a live
 * racer.  The prefix belongs to the common race-object state; the named tail
 * is the bike-specific configuration read by load_rider_physics_configuration.
 */
typedef struct RiderPhysicsConfiguration
{
  unsigned char race_object_prefix[0x168];
  int minimum_acceleration;
  int contact_threshold;
  int bounce_scale;
  unsigned char collision_configuration[0x40];
  int collision_response_limit;
  int collision_response_scale;
  unsigned char bike_class;
  unsigned char bike_class_alignment[3];
  int retained_specification_word_0;
  int retained_specification_word_1;
  RiderGearProfile gears[RIDER_GEAR_PROFILE_COUNT];
  unsigned char gear_runtime_gap[0x10];
  int current_gear;
  int longitudinal_control;
  unsigned char drive_disabled;
  unsigned char drive_disabled_alignment[3];
  int base_engine_pitch;
  int idle_engine_pitch;
  int forward_speed_factor;
  int forward_acceleration_divisor;
  int forward_deceleration_divisor;
  int steering_speed_scale;
  int maximum_forward_control;
  int forward_acceleration_step;
  int forward_deceleration_step;
  int reverse_speed_factor;
  int reverse_acceleration_divisor;
  int reverse_deceleration_divisor;
  unsigned char action_flags;
  unsigned char action_flags_alignment[3];
  int maximum_reverse_control;
  int reverse_acceleration_step;
  int reverse_deceleration_step;
  int steering_angle;
  int previous_steering_angle;
  int slide_activation_threshold;
  int slide_grip_scale_8_8;
  int slide_duration_ticks;
  int steering_limit;
  int steering_acceleration_divisor;
  int steering_deceleration_divisor;
  int steering_acceleration_step;
  int steering_deceleration_step;
  int steering_velocity_scale;
  int steering_return_scale;
  int surface_heading_scale_8_8;
  int alternate_surface_heading_scale_8_8;
  int steering_surface_scale;
  int transmission_reset_ticks;
  int transmission_state;
  int retained_drive_word;
  int left_steering_rate_scale_8_8;
  int right_steering_rate_scale_8_8;
  int collision_steering_gain_8_8;
  int engine_pitch;
  int engine_gain;
  int retained_engine_word;
  int previous_engine_forward_velocity_target;
  int maximum_bike_health;
  int current_bike_health;
} RiderPhysicsConfiguration;

/*
 * Live riders retain one additional terminal transmission profile in the
 * 16-byte runtime slot immediately after the six SPEC-backed profiles.
 */
typedef struct RiderTransmissionRuntime
{
  unsigned char race_object_prefix[0x1C8];
  RiderGearProfile profiles[RIDER_GEAR_RUNTIME_PROFILE_COUNT];
  int current_gear;
} RiderTransmissionRuntime;

typedef char RiderGearProfileSizeCheck[
    (sizeof(RiderGearProfile) == 0x10) ? 1 : -1];
typedef char RiderPhysicsMinimumAccelerationOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, minimum_acceleration) == 0x168)
        ? 1 : -1];
typedef char RiderPhysicsGearProfilesOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, gears) == 0x1C8) ? 1 : -1];
typedef char RiderPhysicsCurrentGearOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, current_gear) == 0x238) ? 1 : -1];
typedef char RiderTransmissionProfilesOffsetCheck[
    (offsetof(RiderTransmissionRuntime, profiles) == 0x1C8) ? 1 : -1];
typedef char RiderTransmissionCurrentGearOffsetCheck[
    (offsetof(RiderTransmissionRuntime, current_gear) == 0x238) ? 1 : -1];
typedef char RiderPhysicsForwardSpeedFactorOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, forward_speed_factor) == 0x24C)
        ? 1 : -1];
typedef char RiderPhysicsReverseSpeedFactorOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, reverse_speed_factor) == 0x268)
        ? 1 : -1];
typedef char RiderPhysicsSteeringAngleOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, steering_angle) == 0x284) ? 1 : -1];
typedef char RiderPhysicsSteeringLimitOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, steering_limit) == 0x298) ? 1 : -1];
typedef char RiderPhysicsSurfaceHeadingScaleOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, surface_heading_scale_8_8) == 0x2B4)
        ? 1 : -1];
typedef char RiderPhysicsEnginePitchOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, engine_pitch) == 0x2D8) ? 1 : -1];
typedef char RiderPhysicsMaximumBikeHealthOffsetCheck[
    (offsetof(RiderPhysicsConfiguration, maximum_bike_health) == 0x2E8)
        ? 1 : -1];
typedef char RiderPhysicsConfigurationSizeCheck[
    (sizeof(RiderPhysicsConfiguration) == RIDER_PHYSICS_CONFIGURATION_SIZE)
        ? 1 : -1];

#endif
