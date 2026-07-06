#ifndef ROAD_PROCEDURAL_CURVE_H
#define ROAD_PROCEDURAL_CURVE_H

#include "bss_late_data.h"

void
generate_random_road_curve_component(int  point_count,
                                     int  target_minimum,
                                     int  target_maximum,
                                     int  maximum_target_delta,
                                     int  minimum_run_length,
                                     int  maximum_run_length,
                                     int *component_output);
void
initialize_procedural_road_curve_profiles(void);

#endif
