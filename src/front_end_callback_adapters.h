#ifndef ROADRASH_FRONT_END_CALLBACK_ADAPTERS_H
#define ROADRASH_FRONT_END_CALLBACK_ADAPTERS_H

void
front_end_advance_random_rider_preview_command_adapter(int first,
                                                       int second);
void
front_end_draw_course_distance_command_adapter(int first,
                                               int second);
void
front_end_draw_current_balance_command_adapter(int first,
                                               int second);
void
front_end_draw_rider_bike_statistics_command_adapter(int first,
                                                     int second);
void
front_end_draw_rider_style_affordability_command_adapter(int first,
                                                         int second);
void
front_end_draw_rider_style_price_command_adapter(int first,
                                                 int second);
void
front_end_draw_selected_bike_summary_command_adapter(int first,
                                                     int second);
void
front_end_draw_selected_rider_summary_command_adapter(int first,
                                                      int second);
void
front_end_finalize_race_setup_command_adapter(int first,
                                              int second);
void
front_end_load_mode_backdrop_command_adapter(int first,
                                             int second);
void
front_end_noop_command_adapter(int first,
                               int second);
void
front_end_play_transition_command_adapter(int first,
                                          int second);
void
front_end_preload_selected_opponent_command_adapter(int first,
                                                    int second);
void
front_end_reset_random_rider_preview_command_adapter(int first,
                                                     int second);
void
front_end_select_available_race_variant_command_adapter(int first,
                                                        int second);
void
front_end_set_selected_level_command_adapter(int first,
                                             int second);
void
front_end_show_current_player_descriptor_command_adapter(int first,
                                                         int second);
void
front_end_show_game_mode_descriptor_command_adapter(int first,
                                                    int second);
void
front_end_show_profile_slot_descriptor_command_adapter(int first,
                                                       int second);
void
front_end_show_race_mode_descriptor_command_adapter(int first,
                                                    int second);
void
front_end_show_rider_type_descriptor_command_adapter(int first,
                                                     int second);
void
front_end_show_selected_level_command_adapter(int first,
                                              int second);
void
front_end_show_selected_player_descriptor_command_adapter(int first,
                                                          int second);
void
front_end_synchronize_current_player_command_adapter(int first,
                                                     int second);
void
front_end_update_rider_progression_command_adapter(int first,
                                                   int second);

int
front_end_can_afford_rider_style_condition_adapter(int first,
                                                   int second);
int
front_end_choose_random_rider_preview_condition_adapter(int first,
                                                        int second);
int
front_end_consume_pending_event_condition_adapter(int first,
                                                  int second);
int
front_end_has_pending_race_result_condition_adapter(int first,
                                                    int second);
int
front_end_is_alternate_game_mode_condition_adapter(int first,
                                                   int second);
int
front_end_is_player_configuration_valid_condition_adapter(int first,
                                                          int second);
int
front_end_is_profile_slot_available_condition_adapter(int first,
                                                      int second);
int
front_end_is_selected_rider_style_condition_adapter(int first,
                                                    int second);
int
front_end_is_single_player_condition_adapter(int first,
                                             int second);
int
front_end_is_special_mode_enabled_condition_adapter(int first,
                                                    int second);
int
front_end_prepare_selected_opponents_condition_adapter(int first,
                                                       int second);
int
front_end_set_player_descriptor_selection_condition_adapter(int first,
                                                            int second);
int
front_end_set_selected_finish_position_condition_adapter(int first,
                                                         int second);

#endif
