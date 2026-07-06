#include "rider_contact_runtime.h"

boolean
is_rider_left_of_target(const RacerEntity *rider,
                        const RacerEntity *target)
{
  int rider_right_edge;
  int target_left_edge;

  rider_right_edge =
    rider->position_z + rider->collision_half_length_z;
  target_left_edge =
    target->position_z - target->collision_half_length_z;
  return (boolean)(rider_right_edge < target_left_edge);
}
