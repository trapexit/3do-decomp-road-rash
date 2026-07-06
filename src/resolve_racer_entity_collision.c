/* resolve_racer_entity_collision.c - pairwise racer collision response */

#include "division_helpers.h"
#include "racer_runtime.h"

enum RacerEntityCollisionFlag
{
  RACER_COLLISION_TRACK_FORWARD = 0x01,
  RACER_COLLISION_TRACK_REARWARD = 0x02,
  RACER_COLLISION_LATERAL_RIGHT = 0x04,
  RACER_COLLISION_LATERAL_LEFT = 0x08,
  RACER_COLLISION_VERTICAL_BELOW = 0x10,
  RACER_COLLISION_VERTICAL_ABOVE = 0x20,
  RACER_COLLISION_VERTICAL_BYPASS = 0x40
};

enum RacerEntityCollisionConstant
{
  RACER_COLLISION_SIGN_SHIFT = 31,
  RACER_COLLISION_QUARTER_SHIFT = 2,
  RACER_COLLISION_QUARTER_BIAS = 3,
  RACER_COLLISION_TRACK_SPEED_SHIFT_PRIMARY = 7,
  RACER_COLLISION_TRACK_SPEED_SHIFT_SECONDARY = 9,
  RACER_COLLISION_TRACK_SPEED_SHIFT_TERTIARY = 12,
  RACER_COLLISION_TRACK_CLEARANCE = 8,
  RACER_COLLISION_POSITION_SLOP = 0x800,
  RACER_COLLISION_POSITION_BIAS = 3,
  RACER_COLLISION_VERTICAL_PLACEMENT =
    RACER_COLLISION_POSITION_SLOP + RACER_COLLISION_POSITION_BIAS,
  RACER_COLLISION_TALL_OBJECT_HEIGHT = 0x2000,
  RACER_COLLISION_LOW_TRACK_SPEED = 0x14,
  RACER_COLLISION_CLASS_RIDER = 4
};

int
compute_elastic_collision_velocity_delta(int first_velocity,
                                         int second_velocity,
                                         int first_mass,
                                         int second_mass);
void
advance_racer_track_position(RacerEntity *racer,
                             int          track_delta,
                             int          accumulate_contact);

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


static
int
collision_absolute_value(int value)
{
  if(value <= 0)
    {
      value = -value;
    }
  return value;
}


static
int
collision_half_towards_zero(int value)
{
  return (value +
          (int)((unsigned int)value >> RACER_COLLISION_SIGN_SHIFT)) >> 1;
}


static
int
collision_quarter_towards_zero(int value)
{
  if(value < 0)
    {
      value += RACER_COLLISION_QUARTER_BIAS;
    }
  return value >> RACER_COLLISION_QUARTER_SHIFT;
}


static
void
add_racer_collision_flags(RacerEntity *racer,
                          unsigned int flags)
{
  racer->collision_flags =
    (unsigned char)(racer->collision_flags | flags);
}


static
void
remove_racer_collision_flags(RacerEntity *racer,
                             unsigned int flags)
{
  racer->collision_flags =
    (unsigned char)(racer->collision_flags & ~flags);
}


static
void
snapshot_racer_collision_position(RacerEntity *racer)
{
  racer->previous_position_z = racer->position_z;
  racer->previous_position_y = racer->position_y;
  racer->previous_position_aux = racer->position_aux;
  racer->previous_position_x = racer->position_x;
  racer->previous_track_segment = racer->track_segment;
}


void
resolve_racer_entity_collision(RacerEntity *first,
                               RacerEntity *second)
{
  int first_mass;
  int second_mass;
  int total_mass;
  int track_speed_delta;
  int delta;
  int overlap;
  int first_height;
  int second_height;
  int separate_vertically;
  int raise_first;
  int temporary;
  int correction;
  int first_velocity;
  int second_velocity;
  int first_opposing_velocity;
  int second_opposing_velocity;
  int first_y;
  int second_y;

  if(first->collision_peer == second || second->collision_peer == first)
    {
      return;
    }

  first_mass = first->collision_mass;
  second->other_collision_mass = first_mass;
  second_mass = second->collision_mass;
  first->other_collision_mass = second_mass;
  total_mass = first_mass + second_mass;

  delta = first->world_velocity.forward - second->world_velocity.forward;
  track_speed_delta =
    (delta >> RACER_COLLISION_TRACK_SPEED_SHIFT_PRIMARY) +
    (delta >> RACER_COLLISION_TRACK_SPEED_SHIFT_SECONDARY) +
    (delta >> RACER_COLLISION_TRACK_SPEED_SHIFT_TERTIARY);
  track_speed_delta = collision_absolute_value(track_speed_delta);

  first->collision_flags = 0;
  second->collision_flags = 0;

  delta = collision_absolute_value(first->previous_position_z -
                                   second->previous_position_z);
  if(delta > first->collision_half_length_z +
     second->collision_half_length_z)
    {
      separate_vertically = 0;
      raise_first = 0;

      second_height = second->collision_half_height_y;
      if(collision_half_towards_zero(second_height) >
         first->collision_half_height_y &&
         first->position_y == 0)
        {
          separate_vertically = 1;
        }
      else if(collision_half_towards_zero(
                first->collision_half_height_y) > second_height &&
              second->position_y == 0)
        {
          separate_vertically = 1;
          raise_first = 1;
        }

      temporary =
        collision_half_towards_zero(first->collision_half_width_x);
      first_y = first->position_x;
      second_y = second->position_x;
      second_height = second->collision_half_width_x;
      if(second_y - second_height < first_y - temporary &&
         first_y + temporary < second_y + second_height &&
         second->collision_half_height_y >
         RACER_COLLISION_TALL_OBJECT_HEIGHT)
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_VERTICAL_BYPASS);
        }

      temporary =
        collision_half_towards_zero(second->collision_half_width_x);
      first_y = first->position_x;
      second_y = second->position_x;
      first_height = first->collision_half_width_x;
      if(first_y - first_height < second_y - temporary &&
         second_y + temporary < first_y + first_height &&
         first->collision_half_height_y >
         RACER_COLLISION_TALL_OBJECT_HEIGHT)
        {
          add_racer_collision_flags(second,
                                    RACER_COLLISION_VERTICAL_BYPASS);
        }

      delta = collision_absolute_value(first->position_z -
                                       second->position_z);
      overlap = first->collision_half_length_z +
                second->collision_half_length_z +
                RACER_COLLISION_TRACK_CLEARANCE - delta;

      if(separate_vertically != 0)
        {
          first_y = first->position_y;
          second_y = second->position_y;
          delta = collision_absolute_value(
            (first_y + first->collision_half_height_y) -
            (second_y + second->collision_half_height_y));
          correction = first->collision_half_height_y +
                       second->collision_half_height_y +
                       RACER_COLLISION_POSITION_SLOP - delta;
          if(raise_first != 0)
            {
              first->position_y =
                first_y + correction + RACER_COLLISION_POSITION_BIAS;
            }
          else
            {
              second->position_y =
                second_y + correction + RACER_COLLISION_POSITION_BIAS;
            }
        }

      if(second->previous_position_z < first->previous_position_z)
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_TRACK_FORWARD);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_TRACK_REARWARD);
          if(separate_vertically == 0)
            {
              correction = signed_divide_quotient(
                total_mass, second_mass * overlap);
              advance_racer_track_position(
                first, correction + RACER_COLLISION_POSITION_BIAS, 0);
              if(first->collision_class != RACER_COLLISION_CLASS_RIDER ||
                 track_speed_delta < RACER_COLLISION_LOW_TRACK_SPEED)
                {
                  correction = signed_divide_quotient(
                    total_mass, first_mass * overlap);
                  advance_racer_track_position(
                    second,
                    -(correction + RACER_COLLISION_POSITION_BIAS), 0);
                }
            }
        }
      else
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_TRACK_REARWARD);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_TRACK_FORWARD);
          if(separate_vertically == 0)
            {
              if(first->collision_class != RACER_COLLISION_CLASS_RIDER ||
                 track_speed_delta < RACER_COLLISION_LOW_TRACK_SPEED)
                {
                  correction = signed_divide_quotient(
                    total_mass, second_mass * overlap);
                  advance_racer_track_position(
                    first,
                    -(correction + RACER_COLLISION_POSITION_BIAS), 0);
                }
              correction = signed_divide_quotient(
                total_mass, first_mass * overlap);
              advance_racer_track_position(
                second, correction + RACER_COLLISION_POSITION_BIAS, 0);
            }
        }

      correction = compute_elastic_collision_velocity_delta(
        first->world_velocity.forward, second->world_velocity.forward,
        first_mass, second_mass);
      first->contact_impulse.forward = correction;
      second->contact_impulse.forward = -correction;
    }
  else
    {
      first->contact_impulse.forward = 0;
      second->contact_impulse.forward = 0;
    }

  delta = collision_absolute_value(first->previous_position_x -
                                   second->previous_position_x);
  temporary = first->collision_half_width_x +
              second->collision_half_width_x;
  if(delta > temporary)
    {
      delta = collision_absolute_value(first->position_x -
                                       second->position_x);
      overlap = temporary + RACER_COLLISION_POSITION_SLOP - delta;
      if(second->previous_position_x < first->previous_position_x)
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_LATERAL_RIGHT);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_LATERAL_LEFT);
          correction = signed_divide_quotient(
            total_mass, second_mass * overlap);
          first->position_x +=
            correction + RACER_COLLISION_POSITION_BIAS;
          correction = signed_divide_quotient(
            total_mass, first_mass * overlap);
          second->position_x -=
            correction + RACER_COLLISION_POSITION_BIAS;
        }
      else
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_LATERAL_LEFT);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_LATERAL_RIGHT);
          correction = signed_divide_quotient(
            total_mass, second_mass * overlap);
          first->position_x -=
            correction + RACER_COLLISION_POSITION_BIAS;
          correction = signed_divide_quotient(
            total_mass, first_mass * overlap);
          second->position_x +=
            correction + RACER_COLLISION_POSITION_BIAS;
        }

      correction = compute_elastic_collision_velocity_delta(
        first->world_velocity.lateral, second->world_velocity.lateral,
        first_mass, second_mass);
      first->contact_impulse.lateral = correction;
      second->contact_impulse.lateral = -correction;
    }
  else
    {
      first->contact_impulse.lateral = 0;
      second->contact_impulse.lateral = 0;
    }

  first_height = first->collision_half_height_y;
  second_height = second->collision_half_height_y;
  delta = collision_absolute_value(
    (first->previous_position_y + first_height) -
    (second->previous_position_y + second_height));
  if(delta > first_height + second_height)
    {
      delta = collision_absolute_value(
        (first->position_y + first_height) -
        (second->position_y + second_height));
      overlap = first_height + second_height +
                RACER_COLLISION_POSITION_SLOP - delta;
      if(second->previous_position_y < first->previous_position_y)
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_VERTICAL_ABOVE);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_VERTICAL_BELOW);
          remove_racer_collision_flags(second,
                                       RACER_COLLISION_VERTICAL_BYPASS);
          correction = signed_divide_quotient(
            total_mass, second_mass * overlap);
          first->position_y +=
            correction + RACER_COLLISION_POSITION_BIAS;
          correction = signed_divide_quotient(
            total_mass, first_mass * overlap);
          temporary = second->position_y -
                      (correction + RACER_COLLISION_POSITION_BIAS);
          second->position_y = temporary;
          if(temporary < 0)
            {
              first->position_y -= temporary;
              second->position_y = 0;
            }
        }
      else
        {
          add_racer_collision_flags(first,
                                    RACER_COLLISION_VERTICAL_BELOW);
          remove_racer_collision_flags(first,
                                       RACER_COLLISION_VERTICAL_BYPASS);
          add_racer_collision_flags(second,
                                    RACER_COLLISION_VERTICAL_ABOVE);
          correction = signed_divide_quotient(
            total_mass, second_mass * overlap);
          first->position_y -=
            correction + RACER_COLLISION_POSITION_BIAS;
          correction = signed_divide_quotient(
            total_mass, first_mass * overlap);
          temporary = second->position_y + correction +
                      RACER_COLLISION_POSITION_BIAS;
          second->position_y = temporary;
          if(first->position_y < 0)
            {
              second->position_y = temporary - first->position_y;
              first->position_y = 0;
            }
        }

      correction = compute_elastic_collision_velocity_delta(
        first->world_velocity.vertical, second->world_velocity.vertical,
        first_mass, second_mass);
      first->contact_impulse.vertical = correction;
      second->contact_impulse.vertical = -correction;
    }
  else
    {
      first->contact_impulse.vertical = 0;
      second->contact_impulse.vertical = 0;
    }

  if(first->collision_flags == 0)
    {
      second_height = second->collision_half_height_y;
      first_height = first->collision_half_height_y;
      if(collision_half_towards_zero(second_height) > first_height &&
         first->position_y == 0)
        {
          raise_first = 0;
        }
      else if(collision_half_towards_zero(first_height) > second_height &&
              second->position_y == 0)
        {
          raise_first = 1;
        }
      else
        {
          first_velocity = compute_elastic_collision_velocity_delta(
            first->world_velocity.forward,
            second->world_velocity.forward, first_mass, second_mass);
          first->contact_impulse.forward = first_velocity;
          first_opposing_velocity = -first_velocity;
          second->contact_impulse.forward = first_opposing_velocity;

          second_velocity = compute_elastic_collision_velocity_delta(
            first->world_velocity.lateral,
            second->world_velocity.lateral, first_mass, second_mass);
          first->contact_impulse.lateral = second_velocity;
          second_opposing_velocity = -second_velocity;
          second->contact_impulse.lateral = second_opposing_velocity;

          temporary = collision_absolute_value(
            collision_quarter_towards_zero(first_velocity));
          correction = collision_absolute_value(second_velocity);
          if(temporary < correction ||
             first->world_velocity.lateral <=
             first->world_velocity.forward)
            {
              first_y = first->position_x;
              delta = collision_absolute_value(
                first_y - second->position_x);
              overlap = first->collision_half_width_x +
                        second->collision_half_width_x +
                        RACER_COLLISION_POSITION_SLOP - delta;
              if(first->collision_class != RACER_COLLISION_CLASS_RIDER)
                {
                  if(second_velocity < 0)
                    {
                      add_racer_collision_flags(
                        first, RACER_COLLISION_LATERAL_LEFT);
                      correction = signed_divide_quotient(
                        total_mass, second_mass * overlap);
                      first->position_x =
                        first_y -
                        (correction + RACER_COLLISION_POSITION_BIAS);
                    }
                  else
                    {
                      add_racer_collision_flags(
                        first, RACER_COLLISION_LATERAL_RIGHT);
                      correction = signed_divide_quotient(
                        total_mass, second_mass * overlap);
                      first->position_x = first_y + correction +
                                          RACER_COLLISION_POSITION_BIAS;
                    }
                }
              if(second->collision_class != RACER_COLLISION_CLASS_RIDER)
                {
                  if(second_opposing_velocity <= 0)
                    {
                      add_racer_collision_flags(
                        second, RACER_COLLISION_LATERAL_LEFT);
                      correction = signed_divide_quotient(
                        total_mass, first_mass * overlap);
                      second->position_x -=
                        correction + RACER_COLLISION_POSITION_BIAS;
                    }
                  else
                    {
                      add_racer_collision_flags(
                        second, RACER_COLLISION_LATERAL_RIGHT);
                      correction = signed_divide_quotient(
                        total_mass, first_mass * overlap);
                      second->position_x +=
                        correction + RACER_COLLISION_POSITION_BIAS;
                    }
                }
            }
          else
            {
              delta = collision_absolute_value(first->position_z -
                                               second->position_z);
              overlap = first->collision_half_length_z +
                        second->collision_half_length_z +
                        RACER_COLLISION_TRACK_CLEARANCE - delta;
              if(first_velocity < 0)
                {
                  add_racer_collision_flags(
                    first, RACER_COLLISION_TRACK_REARWARD);
                  if(first->collision_class !=
                     RACER_COLLISION_CLASS_RIDER ||
                     track_speed_delta < RACER_COLLISION_LOW_TRACK_SPEED)
                    {
                      correction = signed_divide_quotient(
                        total_mass, second_mass * overlap);
                      advance_racer_track_position(
                        first,
                        -(correction + RACER_COLLISION_POSITION_BIAS), 0);
                    }
                }
              else
                {
                  add_racer_collision_flags(
                    first, RACER_COLLISION_TRACK_FORWARD);
                  correction = signed_divide_quotient(
                    total_mass, second_mass * overlap);
                  advance_racer_track_position(
                    first, correction + RACER_COLLISION_POSITION_BIAS, 0);
                }

              if(first_opposing_velocity <= 0)
                {
                  add_racer_collision_flags(
                    second, RACER_COLLISION_TRACK_REARWARD);
                  if(second->collision_class !=
                     RACER_COLLISION_CLASS_RIDER ||
                     track_speed_delta < RACER_COLLISION_LOW_TRACK_SPEED)
                    {
                      correction = signed_divide_quotient(
                        total_mass, first_mass * overlap);
                      advance_racer_track_position(
                        second,
                        -(correction + RACER_COLLISION_POSITION_BIAS), 0);
                    }
                }
              else
                {
                  add_racer_collision_flags(
                    second, RACER_COLLISION_TRACK_FORWARD);
                  correction = signed_divide_quotient(
                    total_mass, first_mass * overlap);
                  advance_racer_track_position(
                    second, correction + RACER_COLLISION_POSITION_BIAS,
                    0);
                }
            }

          snapshot_racer_collision_position(first);
          snapshot_racer_collision_position(second);
          collision_positions_adjusted[0] = 1;
          return;
        }

      first_y = first->position_y;
      second_y = second->position_y;
      delta = collision_absolute_value((first_y + first_height) -
                                       (second_y + second_height));
      overlap = first_height + second_height - delta;
      if(raise_first != 0)
        {
          second_y = first_y;
        }
      temporary = second_y + overlap +
                  RACER_COLLISION_VERTICAL_PLACEMENT;
      if(raise_first == 0)
        {
          second->position_y = temporary;
        }
      else
        {
          first->position_y = temporary;
        }
    }

  snapshot_racer_collision_position(first);
  snapshot_racer_collision_position(second);
  collision_positions_adjusted[0] = 1;
}
