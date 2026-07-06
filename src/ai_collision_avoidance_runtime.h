#ifndef ROADRASH_AI_COLLISION_AVOIDANCE_RUNTIME_H
#define ROADRASH_AI_COLLISION_AVOIDANCE_RUNTIME_H

#include "ai_racer_runtime.h"
#include "track_object_runtime.h"

int
average_road_lane_curvature(RoadSegmentLaneRuntime *lane,
                            int                     sample_count);
int
collision_avoidance_lateral_width(const TrackInteractionObject *object);
int
calculate_track_position_bearing(const TrackObjectPosition *origin,
                                 const TrackObjectPosition *target);
TrackInteractionObject *
find_collision_intercept_target(TrackInteractionObject *origin,
                                TrackInteractionObject *excluded,
                                int                     object_mask,
                                int                     search_mode,
                                int                     maximum_intercept_time,
                                int                     maximum_distance,
                                int                     lateral_window_width,
                                int                    *intercept_time);
int
calculate_ai_curve_speed_target(AiRacerObject *racer);
int
select_ai_collision_avoidance_target(AiRacerObject *racer);
int
select_ai_lane_target(AiRacerObject *racer);
int
select_ai_curvature_lane_target(AiRacerObject *racer);
int
apply_ai_rider_navigation_control(RacerEntity *rider);
void
no_op_return(void);

#endif
