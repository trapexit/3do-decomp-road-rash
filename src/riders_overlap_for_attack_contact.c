#include "rider_contact_runtime.h"

enum RiderAttackOverlapValue
{
  RIDER_ATTACK_DIRECTION_BOOLEAN_MASK = 0xFF,
  RIDER_ATTACK_LONGITUDINAL_DISTANCE_LIMIT = 0x40,
  RIDER_ATTACK_PUNCH_OR_KICK_REACH = 5,
  RIDER_ATTACK_CLUB_REACH = 7,
  RIDER_ATTACK_CHAIN_REACH = 8,
  RIDER_ATTACK_LATERAL_REACH_SHIFT = 12,
  RIDER_ATTACK_VERTICAL_DISTANCE_LIMIT = 0x3000
};

boolean
riders_overlap_for_attack_contact(const RacerEntity *rider,
                                  const RacerEntity *target_object,
                                  boolean            attacks_right,
                                  RiderAttackStyle   attack_style)
{
  int rider_lateral_position;
  int target_lateral_position;
  int distance;
  int lateral_reach;

  rider_lateral_position = rider->position_x;
  target_lateral_position = target_object->position_x;
  if(((target_lateral_position > rider_lateral_position) ? true : false) !=
     (attacks_right & RIDER_ATTACK_DIRECTION_BOOLEAN_MASK))
    {
      return false;
    }

  distance = rider->position_z - target_object->position_z;
  if(distance <= 0)
    {
      distance = -distance;
    }
  if(distance >= RIDER_ATTACK_LONGITUDINAL_DISTANCE_LIMIT)
    {
      return false;
    }

  distance = rider_lateral_position - target_lateral_position;
  if(distance <= 0)
    {
      distance = -distance;
    }

  lateral_reach = RIDER_ATTACK_PUNCH_OR_KICK_REACH;
  if(attack_style == RIDER_ATTACK_STYLE_CLUB)
    {
      lateral_reach = RIDER_ATTACK_CLUB_REACH;
    }
  else if(attack_style == RIDER_ATTACK_STYLE_CHAIN)
    {
      lateral_reach = RIDER_ATTACK_CHAIN_REACH;
    }
  if(distance >=
     (lateral_reach << RIDER_ATTACK_LATERAL_REACH_SHIFT))
    {
      return false;
    }

  distance = rider->position_y - target_object->position_y;
  if(distance <= 0)
    {
      distance = -distance;
    }

  return distance < RIDER_ATTACK_VERTICAL_DISTANCE_LIMIT;
}
