/* Reconstructed from original code range 0x0000E4DC..0x0000E68C. */

#include "division_helpers.h"
#include "race_audio_runtime.h"

enum SpatialAudioParameterConstant
{
  INTEGER_SIGN_SHIFT = 31,
  HALF_FRACTION_SHIFT = 1,
  PAN_FRACTION_SHIFT = 4,
  PAN_FRACTION_ROUNDING = 0x0F,
  BYTE_FRACTION_SHIFT = 8,
  BYTE_FRACTION_ROUNDING = 0xFF,
  ANGLE_FRACTION_SHIFT = 16,
  ANGLE_FRACTION_ROUNDING = 0xFFFF,
  SPATIAL_TRACK_TOPOLOGY_MASK = 3,
  SPATIAL_GAIN_DISTANCE_SHIFT = 3,
  SPATIAL_AUDIBLE_DISTANCE = 0x1E00,
  SPATIAL_DEFAULT_PITCH = 0x8000,
  SPATIAL_PITCH_RATIO_SCALE = 0x8000,
  SPATIAL_GAIN_PEAK = 0x7FFF,
  SPATIAL_VELOCITY_DIVISOR_BASE = 0x13B00,
  SPATIAL_VELOCITY_RATIO_NUMERATOR = 0x013B0000,
  SPATIAL_ANGLE_REAR_LIMIT = 0xC0,
  SPATIAL_ANGLE_FULL_TURN = 0x100,
  SPATIAL_ANGLE_SIDE_LIMIT = 0x40,
  SPATIAL_ANGLE_HALF_TURN = 0x80,
  SPATIAL_PAN_ANGLE_SCALE = 0x0D,
  SPATIAL_PAN_CENTER = 0x40
};

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

static
int
divide_by_two_toward_zero(int value)
{
  value += (int)((unsigned int)value >> INTEGER_SIGN_SHIFT);
  return value >> HALF_FRACTION_SHIFT;
}


static
int
divide_by_sixteen_toward_zero(int value)
{
  if(value < 0)
    {
      value += PAN_FRACTION_ROUNDING;
    }
  return value >> PAN_FRACTION_SHIFT;
}


static
int
divide_by_256_toward_zero(int value)
{
  if(value < 0)
    {
      value += BYTE_FRACTION_ROUNDING;
    }
  return value >> BYTE_FRACTION_SHIFT;
}


static
int
divide_by_65536_toward_zero(int value)
{
  if(value < 0)
    {
      value += ANGLE_FRACTION_ROUNDING;
    }
  return value >> ANGLE_FRACTION_SHIFT;
}


void
calculate_spatial_audio_parameters(const RacerEntity       *source_object,
                                   const SpatialAudioEvent *event,
                                   int                     *out_pitch,
                                   int                     *out_volume,
                                   int                     *out_pan)
{
  const RacerEntity *player_object;
  const RoadSegmentLaneRuntime *source_track;
  const RoadSegmentLaneRuntime *player_track;
  SignedDivisionResult pitch_ratio;
  int longitudinal_distance;
  int absolute_longitudinal_distance;
  int lateral_distance;
  int absolute_lateral_distance;
  int shorter_distance;
  int combined_distance;
  int distance_gain;
  int relative_velocity;
  int direction_angle;
  int scaled_gain;

  longitudinal_distance =
    event->position_z -
    track_simulation_globals.motion_source.packed_track_position;
  absolute_longitudinal_distance = longitudinal_distance;
  if(absolute_longitudinal_distance < 0)
    {
      absolute_longitudinal_distance = -absolute_longitudinal_distance;
    }

  if(absolute_longitudinal_distance >= SPATIAL_AUDIBLE_DISTANCE)
    {
      goto inaudible;
    }

  lateral_distance = divide_by_256_toward_zero(event->position_x) -
                     track_simulation_globals.motion_source.projection_origin_x;
  if(source_object != 0)
    {
      source_track = source_object->track_segment;
      if(source_track == 0)
        {
          goto inaudible;
        }

      player_object = (const RacerEntity *)race_rider_state.player_object;
      player_track = player_object->track_segment;
      if((source_track->topology_flags & player_track->topology_flags &
          SPATIAL_TRACK_TOPOLOGY_MASK) == 0)
        {
          lateral_distance +=
            calculate_track_relative_lateral_offset(source_object);
        }
    }

  absolute_lateral_distance = lateral_distance;
  if(absolute_lateral_distance < 0)
    {
      absolute_lateral_distance = -absolute_lateral_distance;
    }

  shorter_distance = absolute_lateral_distance;
  if(absolute_longitudinal_distance < absolute_lateral_distance)
    {
      shorter_distance = absolute_longitudinal_distance;
    }
  combined_distance = absolute_longitudinal_distance +
                      absolute_lateral_distance -
                      divide_by_two_toward_zero(shorter_distance);
  if(combined_distance >= SPATIAL_AUDIBLE_DISTANCE)
    {
      goto inaudible;
    }

  distance_gain = divide_by_256_toward_zero(
    (SPATIAL_AUDIBLE_DISTANCE - combined_distance)
      << SPATIAL_GAIN_DISTANCE_SHIFT);
  if(longitudinal_distance < 0)
    {
      relative_velocity =
        event->forward_velocity -
        track_simulation_globals.motion_source.source_position_z;
    }
  else
    {
      relative_velocity =
        track_simulation_globals.motion_source.source_position_z -
        event->forward_velocity;
    }

  pitch_ratio = signed_divide_with_remainder(
    (unsigned int)(SPATIAL_VELOCITY_DIVISOR_BASE - relative_velocity),
    (unsigned int)SPATIAL_VELOCITY_RATIO_NUMERATOR);
  direction_angle = fixed_vector_angle_24(longitudinal_distance,
                                          lateral_distance);
  direction_angle = divide_by_65536_toward_zero(direction_angle);
  if(direction_angle > SPATIAL_ANGLE_REAR_LIMIT)
    {
      direction_angle -= SPATIAL_ANGLE_FULL_TURN;
    }
  else if(direction_angle > SPATIAL_ANGLE_SIDE_LIMIT)
    {
      direction_angle = SPATIAL_ANGLE_HALF_TURN - direction_angle;
    }

  *out_pitch = divide_by_256_toward_zero(
    pitch_ratio.quotient * SPATIAL_PITCH_RATIO_SCALE);

  scaled_gain = divide_by_256_toward_zero(
    distance_gain * SPATIAL_GAIN_PEAK);
  *out_volume = divide_by_256_toward_zero(distance_gain * scaled_gain);

  *out_pan = divide_by_sixteen_toward_zero(
    direction_angle * SPATIAL_PAN_ANGLE_SCALE) +
             SPATIAL_PAN_CENTER;
  return;

inaudible:
  *out_pitch = SPATIAL_DEFAULT_PITCH;
  *out_volume = 0;
}
