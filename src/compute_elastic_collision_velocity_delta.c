#include "division_helpers.h"

int
compute_elastic_collision_velocity_delta(int first_velocity,
                                         int second_velocity,
                                         int first_mass,
                                         int second_mass)
{
  SignedDivisionResult division;
  int total_mass;
  int weighted_velocity;

  total_mass = first_mass + second_mass;
  weighted_velocity = first_velocity * (first_mass - second_mass) +
                      second_velocity * (second_mass << 1);
  division = signed_divide_with_remainder((unsigned int)total_mass,
                                          (unsigned int)weighted_velocity);
  return division.quotient - first_velocity;
}
