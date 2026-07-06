#include "division_helpers.h"
#include "race_audio_runtime.h"

enum RacerEngineSpatialAudioConstant
{
  ENGINE_SPATIAL_INTEGER_SIGN_SHIFT = 31,
  ENGINE_SPATIAL_HALF_SHIFT = 1,
  ENGINE_SPATIAL_PAN_SHIFT = 4,
  ENGINE_SPATIAL_PAN_ROUNDING = 0x0F,
  ENGINE_SPATIAL_BYTE_SHIFT = 8,
  ENGINE_SPATIAL_BYTE_ROUNDING = 0xFF,
  ENGINE_SPATIAL_ANGLE_SHIFT = 16,
  ENGINE_SPATIAL_ANGLE_ROUNDING = 0xFFFF,
  ENGINE_SPATIAL_TRACK_TOPOLOGY_MASK = 3,
  ENGINE_SPATIAL_GAIN_DISTANCE_SHIFT = 3,
  ENGINE_SPATIAL_AUDIBLE_DISTANCE = 0x1E00,
  ENGINE_SPATIAL_PITCH_DIVISOR_BASE = 0x13B00,
  ENGINE_SPATIAL_PITCH_RATIO_NUMERATOR = 0x013B0000,
  ENGINE_SPATIAL_VECTOR_ANGLE_FORWARD_LIMIT = 0x300,
  ENGINE_SPATIAL_ANGLE_REAR_LIMIT = 0xC0,
  ENGINE_SPATIAL_ANGLE_FULL_TURN = 0x100,
  ENGINE_SPATIAL_ANGLE_SIDE_LIMIT = 0x40,
  ENGINE_SPATIAL_ANGLE_HALF_TURN = 0x80,
  ENGINE_SPATIAL_PAN_ANGLE_SCALE = 0x0D,
  ENGINE_SPATIAL_DEFAULT_PITCH_SCALE_8_8 = 0x100
};

int
fixed_vector_angle_24(int horizontal,
                      int vertical);

static
int
divide_engine_spatial_by_two_toward_zero(int value)
{
  value +=
    (int)((unsigned int)value >> ENGINE_SPATIAL_INTEGER_SIGN_SHIFT);
  return value >> ENGINE_SPATIAL_HALF_SHIFT;
}


static
int
divide_engine_spatial_by_sixteen_toward_zero(int value)
{
  if(value < 0)
    {
      value += ENGINE_SPATIAL_PAN_ROUNDING;
    }
  return value >> ENGINE_SPATIAL_PAN_SHIFT;
}


static
int
divide_engine_spatial_by_256_toward_zero(int value)
{
  if(value < 0)
    {
      value += ENGINE_SPATIAL_BYTE_ROUNDING;
    }
  return value >> ENGINE_SPATIAL_BYTE_SHIFT;
}


static
int
divide_engine_spatial_by_65536_toward_zero(int value)
{
  if(value < 0)
    {
      value += ENGINE_SPATIAL_ANGLE_ROUNDING;
    }
  return value >> ENGINE_SPATIAL_ANGLE_SHIFT;
}


void
update_racer_engine_audio_spatial_state(RacerEntity *racer)
{
  RendererMotionState *listener_motion;
  const RacerEntity *player;
  RoadSegmentLaneRuntime *racer_track;
  RoadSegmentLaneRuntime *player_track;
  SignedDivisionResult pitch_ratio;
  int longitudinal_distance;
  int absolute_longitudinal_distance;
  int lateral_distance;
  int absolute_lateral_distance;
  int shorter_distance;
  int combined_distance;
  int distance_gain;
  int relative_forward_motion;
  int direction_angle;

  if(racer == 0)
    {
      return;
    }
  racer_track = racer->track_segment;

  if(racer_track != 0)
    {
      listener_motion = &track_simulation_globals.motion_source;
      longitudinal_distance =
        racer->position_z - listener_motion->packed_track_position;
      absolute_longitudinal_distance = longitudinal_distance;
      if(absolute_longitudinal_distance < 0)
        {
          absolute_longitudinal_distance =
            -absolute_longitudinal_distance;
        }

      if(absolute_longitudinal_distance <
         ENGINE_SPATIAL_AUDIBLE_DISTANCE)
        {
          lateral_distance = divide_engine_spatial_by_256_toward_zero(
            racer->position_x) -
                             listener_motion->projection_origin_x;

          player = (const RacerEntity *)race_rider_state.player_object;
          player_track = player->track_segment;
          if((racer_track->topology_flags &
              player_track->topology_flags &
              ENGINE_SPATIAL_TRACK_TOPOLOGY_MASK) == 0)
            {
              lateral_distance +=
                calculate_track_relative_lateral_offset(racer);
            }

          absolute_lateral_distance = lateral_distance;
          if(absolute_lateral_distance < 0)
            {
              absolute_lateral_distance = -absolute_lateral_distance;
            }

          shorter_distance = absolute_lateral_distance;
          if(absolute_longitudinal_distance <
             absolute_lateral_distance)
            {
              shorter_distance = absolute_longitudinal_distance;
            }
          combined_distance =
            absolute_longitudinal_distance +
            absolute_lateral_distance -
            divide_engine_spatial_by_two_toward_zero(
              shorter_distance);

          if(combined_distance < ENGINE_SPATIAL_AUDIBLE_DISTANCE)
            {
              distance_gain =
                divide_engine_spatial_by_256_toward_zero(
                  (ENGINE_SPATIAL_AUDIBLE_DISTANCE -
                   combined_distance)
                    << ENGINE_SPATIAL_GAIN_DISTANCE_SHIFT);

              if(longitudinal_distance < 0)
                {
                  relative_forward_motion =
                    racer->world_velocity.forward -
                    listener_motion->source_position_z;
                  racer->spatial_longitudinal_side = 1;
                }
              else
                {
                  relative_forward_motion =
                    listener_motion->source_position_z -
                    racer->world_velocity.forward;
                  racer->spatial_longitudinal_side = 0;
                }

              pitch_ratio = signed_divide_with_remainder(
                (unsigned int)(ENGINE_SPATIAL_PITCH_DIVISOR_BASE -
                               relative_forward_motion),
                (unsigned int)ENGINE_SPATIAL_PITCH_RATIO_NUMERATOR);

              if(longitudinal_distance <
                 ENGINE_SPATIAL_VECTOR_ANGLE_FORWARD_LIMIT)
                {
                  direction_angle = fixed_vector_angle_24(
                    longitudinal_distance, lateral_distance);
                }
              else
                {
                  direction_angle = racer->render_heading;
                }
              direction_angle =
                divide_engine_spatial_by_65536_toward_zero(
                  direction_angle);
              if(direction_angle > ENGINE_SPATIAL_ANGLE_REAR_LIMIT)
                {
                  direction_angle -= ENGINE_SPATIAL_ANGLE_FULL_TURN;
                }
              else if(direction_angle >
                      ENGINE_SPATIAL_ANGLE_SIDE_LIMIT)
                {
                  direction_angle =
                    ENGINE_SPATIAL_ANGLE_HALF_TURN - direction_angle;
                }

              racer->spatial_pitch_scale_8_8 = pitch_ratio.quotient;
              racer->spatial_gain_scale_8_8 =
                divide_engine_spatial_by_256_toward_zero(
                  distance_gain * distance_gain);
              racer->spatial_pan_offset =
                divide_engine_spatial_by_sixteen_toward_zero(
                  direction_angle * ENGINE_SPATIAL_PAN_ANGLE_SCALE);
              return;
            }
        }
    }

  racer->spatial_pitch_scale_8_8 =
    ENGINE_SPATIAL_DEFAULT_PITCH_SCALE_8_8;
  racer->spatial_gain_scale_8_8 = 0;
}
