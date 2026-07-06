#include "racer_runtime.h"

enum RiderEnginePitchConstants
{
  RIDER_ENGINE_GEAR_RUNTIME_OFFSET = 0x1C8,
  RIDER_ENGINE_GEAR_PROFILE_COUNT = 7,
  RIDER_ENGINE_NORMAL_MODE = 0,
  RIDER_ENGINE_AIRBORNE_HEIGHT_THRESHOLD = 0x64,
  RIDER_ENGINE_TRANSITION_STEP = 0x1000,
  RIDER_ENGINE_BASE_PITCH = 0x2000,
  RIDER_ENGINE_MAXIMUM_PITCH_CHANGE = 0x2000,
  RIDER_ENGINE_MAXIMUM_PITCH = 0xFFFF,
  RIDER_ENGINE_MAXIMUM_GAIN = 0x7FFF,
  RIDER_ENGINE_PITCH_SCALE_ROUND_BIAS = 0xFF,
  RIDER_ENGINE_PITCH_SCALE_SHIFT = 8
};

typedef struct RiderEngineGearProfile
{
  int pitch_per_forward_velocity_8_8;
  int acceleration_scale_16_16;
  int upshift_forward_velocity;
  int downshift_forward_velocity;
} RiderEngineGearProfile;

typedef struct RiderEngineGearRuntime
{
  RiderEngineGearProfile profiles[RIDER_ENGINE_GEAR_PROFILE_COUNT];
  int active_profile_index;
} RiderEngineGearRuntime;

typedef char RiderEngineGearProfileSizeCheck[
    (sizeof(RiderEngineGearProfile) == 0x10) ? 1 : -1];
typedef char RiderEngineGearIndexOffsetCheck[
    (offsetof(RiderEngineGearRuntime, active_profile_index) == 0x70)
        ? 1 : -1];

void
update_rider_engine_pitch(RacerEntity *rider)
{
  const RiderEngineGearRuntime *gear_runtime;
  const RiderEngineGearProfile *gear_profile;
  int forward_velocity;
  int previous_forward_velocity;
  int current_pitch;
  int target_pitch;
  int pitch_product;
  int pitch_limit;

  forward_velocity = rider->velocity.forward;
  previous_forward_velocity =
    rider->previous_engine_forward_velocity_target;
  if(previous_forward_velocity == forward_velocity &&
     forward_velocity != 0)
    {
      return;
    }

  if(rider->position_y > RIDER_ENGINE_AIRBORNE_HEIGHT_THRESHOLD &&
     forward_velocity != 0)
    {
      target_pitch = rider->engine_pitch + RIDER_ENGINE_TRANSITION_STEP;
      if(target_pitch > RIDER_ENGINE_MAXIMUM_PITCH)
        {
          target_pitch = RIDER_ENGINE_MAXIMUM_PITCH;
        }
    }
  else
    {
      rider->previous_engine_forward_velocity_target = forward_velocity;
      gear_runtime = (const RiderEngineGearRuntime *)(
        (const char *)rider + RIDER_ENGINE_GEAR_RUNTIME_OFFSET);
      gear_profile =
        &gear_runtime->profiles[gear_runtime->active_profile_index];
      pitch_product =
        gear_profile->pitch_per_forward_velocity_8_8 * forward_velocity;
      if(pitch_product < 0)
        {
          pitch_product += RIDER_ENGINE_PITCH_SCALE_ROUND_BIAS;
        }
      target_pitch =
        (pitch_product >> RIDER_ENGINE_PITCH_SCALE_SHIFT) +
        RIDER_ENGINE_BASE_PITCH;

      current_pitch = rider->engine_pitch;
      if(rider->rider_mode != RIDER_ENGINE_NORMAL_MODE &&
         current_pitch > RIDER_ENGINE_BASE_PITCH)
        {
          target_pitch = current_pitch - RIDER_ENGINE_TRANSITION_STEP;
        }
      else
        {
          pitch_limit =
            current_pitch + RIDER_ENGINE_MAXIMUM_PITCH_CHANGE;
          if(pitch_limit < target_pitch)
            {
              target_pitch = pitch_limit;
            }
          else
            {
              pitch_limit =
                current_pitch - RIDER_ENGINE_MAXIMUM_PITCH_CHANGE;
              if(pitch_limit > target_pitch)
                {
                  target_pitch = pitch_limit;
                }
            }
        }
    }

  rider->engine_pitch = target_pitch;
  rider->engine_gain = RIDER_ENGINE_MAXIMUM_GAIN;
}
