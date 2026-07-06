#ifndef ROADRASH_RIDER_CONTACT_RUNTIME_H
#define ROADRASH_RIDER_CONTACT_RUNTIME_H

#include "racer_runtime.h"
#include "Types.h"

typedef enum RiderAttackStyle
{
  RIDER_ATTACK_STYLE_PUNCH = 0,
  RIDER_ATTACK_STYLE_CLUB = 1,
  RIDER_ATTACK_STYLE_CHAIN = 2,
  RIDER_ATTACK_STYLE_KICK = 3,
  RIDER_ATTACK_STYLE_COUNT = 4
} RiderAttackStyle;

void
accumulate_rider_contact_response(RacerEntity *recipient,
                                  RacerEntity *source,
                                  int          response_slot);
void
select_rider_update_handler(RacerEntity *rider);
void
set_rider_update_handler(RacerEntity *rider,
                         int          handler_index,
                         RacerEntity *target);
boolean
is_rider_left_of_target(const RacerEntity *rider,
                        const RacerEntity *target);
boolean
can_rider_start_attack(RacerEntity *rider,
                       RacerEntity *target);
int
select_rider_attack_animation(RacerEntity *rider,
                              RacerEntity *target,
                              int          alternate_variant);
int32
apply_rider_contact_damage(RacerEntity *attacker,
                           RacerEntity *target);
int
apply_rider_collision_damage(RacerEntity *rider,
                             int          rider_damage,
                             int          bike_damage,
                             int          recovery_kind);
RacerEntity *
refresh_rider_attack_target(RacerEntity *rider);
boolean
is_rider_attack_animation(const RacerEntity *rider);
boolean
is_rider_low_attack_animation(const RacerEntity *rider);
boolean
is_rider_left_attack_animation(const RacerEntity *rider);
boolean
is_rider_right_attack_animation(const RacerEntity *rider);
void
resolve_rider_attack_contact(RacerEntity *rider);
boolean
riders_overlap_for_attack_contact(const RacerEntity *rider,
                                  const RacerEntity *target_object,
                                  boolean            attacks_right,
                                  RiderAttackStyle   attack_style);

#endif
