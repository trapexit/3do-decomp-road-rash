#ifndef ROADRASH_RIDER_ANIMATION_RUNTIME_H
#define ROADRASH_RIDER_ANIMATION_RUNTIME_H

#include "racer_runtime.h"

int
initialize_rider_animation(RiderAnimationRuntime *animation,
                           RacerRole              role);
int
advance_car_animation(RiderAnimationRuntime *animation);
int
initialize_car_animation(RiderAnimationRuntime *animation,
                         int                    animation_index);
int
get_car_animation_output_flags(int animation_index);
int
set_primary_rider_animation(RiderAnimationRuntime *animation,
                            int                    state);
int
set_secondary_rider_animation(RiderAnimationRuntime *animation,
                              int                    state);
int
select_primary_rider_animation(RiderAnimationRuntime *animation,
                               int                    state);
int
select_secondary_rider_animation(RiderAnimationRuntime *animation,
                                 int                    state);

#endif
