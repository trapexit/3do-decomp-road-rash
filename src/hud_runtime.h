#ifndef ROADRASH_HUD_RUNTIME_H
#define ROADRASH_HUD_RUNTIME_H

#include "rw_semantic_data.h"

void
draw_speedometer_needle(int angle_256ths,
                        int anchor_x,
                        int anchor_y);
void
draw_hud_health_gauge_needle(int angle_256ths,
                             int anchor_x,
                             int anchor_y,
                             int pivot_x,
                             int pivot_y);
void
draw_hud_health_gauge_animation_frame(int                  frame_index,
                                      const HudCoordinate *position);
void
draw_player_rider_health_gauge(void);
void
draw_player_bike_health_gauge(void);
void
draw_tracked_rider_health_gauge(void);
void
draw_player_speedometer_needle(void);
void
draw_player_speed_readout(void);
void
draw_player_progress_readout(void);
void
draw_player_engine_pitch_needle(void);
void
draw_player_race_position_readout(void);
int32 *
draw_player_rider_portrait(void);
void
draw_player_race_status_panel(void);
void
draw_race_boost_charge_bar(void);

#endif
