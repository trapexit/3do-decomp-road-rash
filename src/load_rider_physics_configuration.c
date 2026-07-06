#include "filestreamfunctions.h"
#include "platform.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "division_helpers.h"
#include "rider_physics_configuration.h"
#include "rw_semantic_data.h"

enum
{
  RIDER_PHYSICS_LINE_BUFFER_SIZE = 0xC8,
  RIDER_PHYSICS_PATH_BUFFER_SIZE = 0x34,
  RIDER_TUNING_BASE = 0x4B,
  RIDER_FIXED_8_8_SHIFT = 8,
  RIDER_QUARTER_TUNING_SHIFT = 2,
  RIDER_QUARTER_NEGATIVE_ROUNDING = 3,
  RIDER_EIGHTH_TUNING_SHIFT = 3,
  RIDER_EIGHTH_NEGATIVE_ROUNDING = 7,
  RIDER_LAST_GEAR = RIDER_GEAR_PROFILE_COUNT - 1,
  RIDER_TOP_GEAR_UPSHIFT_SENTINEL = 0x00FFFFFF,
  RIDER_FIRST_GEAR_DOWNSHIFT_SENTINEL = -0x3E7,
  RIDER_STEERING_REFERENCE = 0x2E00,
  RIDER_RATE_DIVISOR_LIMIT = 0x0F,
  RIDER_TRANSMISSION_RESET_TICKS = 0x14
};

static
int
signed_divide_quotient(int divisor,
                       int numerator)
{
  SignedDivisionResult result;

  result = signed_divide_with_remainder(
    (unsigned int)divisor, (unsigned int)numerator);
  return result.quotient;
}


Stream *
File_Open(char       *path,
          const char *mode);
int
File_Close(Stream *stream);
char *
File_GetLine(char   *buffer,
             int     capacity,
             Stream *stream);

int
File_GetNumber(Stream *stream)
{
  char line[RIDER_PHYSICS_LINE_BUFFER_SIZE];

  while(File_GetLine(line, sizeof(line), stream) != 0)
    {
      if((line[0] >= '0' && line[0] <= '9') || line[0] == '-')
        {
          return atoi(line);
        }
    }

  printf("### ERROR: File_GetNumber() - Reading number\n");
  return 0;
}


void
load_rider_physics_configuration(RiderPhysicsConfiguration *configuration,
                                 char                      *file_name,
                                 int                        tuning_offset)
{
  char path[RIDER_PHYSICS_PATH_BUFFER_SIZE];
  Stream *stream;
  int scale;
  int divisor;
  int numerator;
  int base_tuning;
  int scaled_forward_speed;
  int gear_index;
  int rate_divisor;

  strcpy(path, (const char *)(int)&(resource_root_path));
  strcat(path, file_name);
  stream = File_Open(path, "r");
  if(stream == 0)
    {
      return;
    }

  configuration->bike_class = (unsigned char)File_GetNumber(stream);
  configuration->minimum_acceleration = File_GetNumber(stream);
  configuration->contact_threshold = File_GetNumber(stream);
  configuration->bounce_scale = File_GetNumber(stream);
  configuration->collision_response_limit = File_GetNumber(stream);
  configuration->collision_response_scale = File_GetNumber(stream);
  base_tuning = File_GetNumber(stream);
  File_GetNumber(stream);

  configuration->forward_speed_factor = File_GetNumber(stream);
  configuration->forward_acceleration_divisor = File_GetNumber(stream);
  configuration->forward_deceleration_divisor = File_GetNumber(stream);
  configuration->steering_speed_scale = File_GetNumber(stream);
  configuration->reverse_speed_factor = File_GetNumber(stream);
  configuration->reverse_acceleration_divisor = File_GetNumber(stream);
  configuration->reverse_deceleration_divisor = File_GetNumber(stream);
  configuration->steering_acceleration_divisor = File_GetNumber(stream);
  configuration->steering_deceleration_divisor = File_GetNumber(stream);
  configuration->left_steering_rate_scale_8_8 = File_GetNumber(stream);
  configuration->right_steering_rate_scale_8_8 = File_GetNumber(stream);
  configuration->steering_velocity_scale = File_GetNumber(stream);
  configuration->steering_return_scale = File_GetNumber(stream);
  configuration->surface_heading_scale_8_8 = File_GetNumber(stream);
  configuration->alternate_surface_heading_scale_8_8 =
    File_GetNumber(stream);
  configuration->steering_surface_scale = File_GetNumber(stream);
  configuration->slide_activation_threshold = File_GetNumber(stream);
  configuration->slide_grip_scale_8_8 = File_GetNumber(stream);
  configuration->slide_duration_ticks = File_GetNumber(stream);
  configuration->collision_steering_gain_8_8 = File_GetNumber(stream);
  configuration->maximum_bike_health = File_GetNumber(stream);

  for(gear_index = 0; gear_index < RIDER_GEAR_PROFILE_COUNT;
      gear_index++)
    {
      configuration->gears[gear_index].engine_pitch =
        File_GetNumber(stream);
      configuration->gears[gear_index].maximum_forward_speed =
        File_GetNumber(stream);
      configuration->gears[gear_index].upshift_speed =
        File_GetNumber(stream);
      configuration->gears[gear_index].downshift_speed =
        File_GetNumber(stream);

      if(gear_index == RIDER_LAST_GEAR)
        {
          configuration->gears[gear_index].upshift_speed =
            RIDER_TOP_GEAR_UPSHIFT_SENTINEL;
        }
      if(gear_index == 0)
        {
          configuration->gears[gear_index].downshift_speed =
            RIDER_FIRST_GEAR_DOWNSHIFT_SENTINEL;
        }
    }

  configuration->base_engine_pitch = File_GetNumber(stream);
  configuration->idle_engine_pitch = File_GetNumber(stream);

  scale = base_tuning + RIDER_TUNING_BASE;
  configuration->forward_speed_factor =
    (configuration->forward_speed_factor * scale)
      << RIDER_FIXED_8_8_SHIFT;
  scaled_forward_speed = configuration->forward_speed_factor;

  configuration->reverse_speed_factor =
    (configuration->reverse_speed_factor * scale)
      << RIDER_FIXED_8_8_SHIFT;
  numerator = configuration->reverse_speed_factor;

  divisor = tuning_offset;
  if(divisor < 0)
    {
      divisor += RIDER_QUARTER_NEGATIVE_ROUNDING;
    }
  divisor = base_tuning + (divisor >> RIDER_QUARTER_TUNING_SHIFT);

  configuration->steering_acceleration_divisor =
    signed_divide_quotient(
      divisor, configuration->steering_acceleration_divisor * scale);
  configuration->steering_deceleration_divisor =
    signed_divide_quotient(
      divisor, configuration->steering_deceleration_divisor * scale);

  configuration->steering_limit = RIDER_STEERING_REFERENCE;
  configuration->steering_acceleration_step =
    signed_divide_quotient(
      configuration->steering_acceleration_divisor,
      RIDER_STEERING_REFERENCE);
  configuration->steering_deceleration_step =
    signed_divide_quotient(
      configuration->steering_deceleration_divisor,
      RIDER_STEERING_REFERENCE);
  configuration->slide_duration_ticks <<= 1;
  configuration->current_bike_health = configuration->maximum_bike_health;

  divisor = tuning_offset - RIDER_TUNING_BASE;
  if(divisor < 0)
    {
      divisor += RIDER_EIGHTH_NEGATIVE_ROUNDING;
    }
  divisor = base_tuning + RIDER_TUNING_BASE +
            (divisor >> RIDER_EIGHTH_TUNING_SHIFT);

  configuration->maximum_forward_control =
    signed_divide_quotient(divisor, scaled_forward_speed);
  configuration->maximum_reverse_control =
    signed_divide_quotient(divisor, numerator);

  rate_divisor = configuration->forward_acceleration_divisor;
  if(rate_divisor >= RIDER_RATE_DIVISOR_LIMIT)
    {
      rate_divisor = RIDER_RATE_DIVISOR_LIMIT;
    }
  configuration->forward_acceleration_step = signed_divide_quotient(
    rate_divisor, configuration->maximum_forward_control);

  rate_divisor = configuration->forward_deceleration_divisor;
  if(rate_divisor >= RIDER_RATE_DIVISOR_LIMIT)
    {
      rate_divisor = RIDER_RATE_DIVISOR_LIMIT;
    }
  configuration->forward_deceleration_step = signed_divide_quotient(
    rate_divisor, configuration->maximum_forward_control);

  rate_divisor = configuration->reverse_acceleration_divisor;
  if(rate_divisor >= RIDER_RATE_DIVISOR_LIMIT)
    {
      rate_divisor = RIDER_RATE_DIVISOR_LIMIT;
    }
  configuration->reverse_acceleration_step = signed_divide_quotient(
    rate_divisor, configuration->maximum_reverse_control);

  rate_divisor = configuration->reverse_deceleration_divisor;
  if(rate_divisor >= RIDER_RATE_DIVISOR_LIMIT)
    {
      rate_divisor = RIDER_RATE_DIVISOR_LIMIT;
    }
  configuration->reverse_deceleration_step = signed_divide_quotient(
    rate_divisor, configuration->maximum_reverse_control);

  configuration->longitudinal_control = 0;
  configuration->steering_angle = 0;
  configuration->transmission_state = 0;
  configuration->action_flags = 0;
  configuration->current_gear = 0;
  configuration->transmission_reset_ticks =
    RIDER_TRANSMISSION_RESET_TICKS;
  configuration->drive_disabled = 0;
  configuration->engine_pitch = configuration->base_engine_pitch;

  File_Close(stream);
}
