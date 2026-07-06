#include "front_end_callback_adapters.h"

void
front_end_load_mode_backdrop(void);
void
front_end_noop(void);
void
front_end_preload_selected_opponent(int slot);
void
front_end_draw_current_balance(void);
void
front_end_draw_rider_bike_statistics(int type);
void
front_end_draw_rider_style_affordability(int style);
void
front_end_draw_rider_style_price(int style);
void
front_end_draw_selected_bike_summary(void);
void
front_end_draw_selected_rider_summary(void);
void
front_end_set_selected_level(int level);
void
front_end_show_selected_level(void);
void
front_end_update_rider_progression(void);
void
front_end_advance_random_rider_preview(void);
void
front_end_show_selected_player_descriptor(void);
void
front_end_reset_random_rider_preview(void);
void
front_end_synchronize_current_player(void);
void
front_end_show_current_player_descriptor(void);
void
front_end_show_race_mode_descriptor(void);
void
front_end_draw_course_distance(unsigned int index);

int
front_end_play_transition(void);
int
front_end_select_available_race_variant(void);
int
front_end_finalize_race_setup(void);
int
front_end_show_game_mode_descriptor(void);
int
front_end_show_profile_slot_descriptor(void);
int
front_end_show_rider_type_descriptor(void);

int
front_end_can_afford_rider_style(int style);
int
front_end_choose_random_rider_preview(int operation,
                                      int selection);
int
front_end_consume_pending_event(void);
int
front_end_has_pending_race_result(void);
int
front_end_is_alternate_game_mode(void);
int
front_end_is_player_configuration_valid(void);
int
front_end_is_profile_slot_available(void);
int
front_end_is_selected_rider_style(int style);
int
front_end_is_single_player(void);
int
front_end_is_special_mode_enabled(void);
int
front_end_prepare_selected_opponents(void);
int
front_end_set_player_descriptor_selection(int value);
int
front_end_set_selected_finish_position(int value);

#define DEFINE_COMMAND_VOID_ZERO(adapter, target) \
        void adapter(int first, int second) \
        { \
          (void)first; \
          (void)second; \
          target(); \
        }

#define DEFINE_COMMAND_VOID_ONE(adapter, target) \
        void adapter(int first, int second) \
        { \
          (void)second; \
          target(first); \
        }

#define DEFINE_COMMAND_INT_ZERO(adapter, target) \
        void adapter(int first, int second) \
        { \
          (void)first; \
          (void)second; \
          (void)target(); \
        }

#define DEFINE_CONDITION_ZERO(adapter, target) \
        int adapter(int first, int second) \
        { \
          (void)first; \
          (void)second; \
          return target(); \
        }

#define DEFINE_CONDITION_ONE(adapter, target) \
        int adapter(int first, int second) \
        { \
          (void)second; \
          return target(first); \
        }

#define DEFINE_CONDITION_TWO(adapter, target) \
        int adapter(int first, int second) \
        { \
          return target(first, second); \
        }

DEFINE_COMMAND_VOID_ZERO(
  front_end_advance_random_rider_preview_command_adapter,
  front_end_advance_random_rider_preview)
DEFINE_COMMAND_VOID_ONE(
  front_end_draw_course_distance_command_adapter,
  front_end_draw_course_distance)
DEFINE_COMMAND_VOID_ZERO(
  front_end_draw_current_balance_command_adapter,
  front_end_draw_current_balance)
DEFINE_COMMAND_VOID_ONE(
  front_end_draw_rider_bike_statistics_command_adapter,
  front_end_draw_rider_bike_statistics)
DEFINE_COMMAND_VOID_ONE(
  front_end_draw_rider_style_affordability_command_adapter,
  front_end_draw_rider_style_affordability)
DEFINE_COMMAND_VOID_ONE(
  front_end_draw_rider_style_price_command_adapter,
  front_end_draw_rider_style_price)
DEFINE_COMMAND_VOID_ZERO(
  front_end_draw_selected_bike_summary_command_adapter,
  front_end_draw_selected_bike_summary)
DEFINE_COMMAND_VOID_ZERO(
  front_end_draw_selected_rider_summary_command_adapter,
  front_end_draw_selected_rider_summary)
DEFINE_COMMAND_INT_ZERO(
  front_end_finalize_race_setup_command_adapter,
  front_end_finalize_race_setup)
DEFINE_COMMAND_VOID_ZERO(
  front_end_load_mode_backdrop_command_adapter,
  front_end_load_mode_backdrop)
DEFINE_COMMAND_VOID_ZERO(front_end_noop_command_adapter, front_end_noop)
DEFINE_COMMAND_INT_ZERO(
  front_end_play_transition_command_adapter,
  front_end_play_transition)
DEFINE_COMMAND_VOID_ONE(
  front_end_preload_selected_opponent_command_adapter,
  front_end_preload_selected_opponent)
DEFINE_COMMAND_VOID_ZERO(
  front_end_reset_random_rider_preview_command_adapter,
  front_end_reset_random_rider_preview)
DEFINE_COMMAND_INT_ZERO(
  front_end_select_available_race_variant_command_adapter,
  front_end_select_available_race_variant)
DEFINE_COMMAND_VOID_ONE(
  front_end_set_selected_level_command_adapter,
  front_end_set_selected_level)
DEFINE_COMMAND_VOID_ZERO(
  front_end_show_current_player_descriptor_command_adapter,
  front_end_show_current_player_descriptor)
DEFINE_COMMAND_INT_ZERO(
  front_end_show_game_mode_descriptor_command_adapter,
  front_end_show_game_mode_descriptor)
DEFINE_COMMAND_INT_ZERO(
  front_end_show_profile_slot_descriptor_command_adapter,
  front_end_show_profile_slot_descriptor)
DEFINE_COMMAND_VOID_ZERO(
  front_end_show_race_mode_descriptor_command_adapter,
  front_end_show_race_mode_descriptor)
DEFINE_COMMAND_INT_ZERO(
  front_end_show_rider_type_descriptor_command_adapter,
  front_end_show_rider_type_descriptor)
DEFINE_COMMAND_VOID_ZERO(
  front_end_show_selected_level_command_adapter,
  front_end_show_selected_level)
DEFINE_COMMAND_VOID_ZERO(
  front_end_show_selected_player_descriptor_command_adapter,
  front_end_show_selected_player_descriptor)
DEFINE_COMMAND_VOID_ZERO(
  front_end_synchronize_current_player_command_adapter,
  front_end_synchronize_current_player)
DEFINE_COMMAND_VOID_ZERO(
  front_end_update_rider_progression_command_adapter,
  front_end_update_rider_progression)

DEFINE_CONDITION_ONE(
  front_end_can_afford_rider_style_condition_adapter,
  front_end_can_afford_rider_style)
DEFINE_CONDITION_TWO(
  front_end_choose_random_rider_preview_condition_adapter,
  front_end_choose_random_rider_preview)
DEFINE_CONDITION_ZERO(
  front_end_consume_pending_event_condition_adapter,
  front_end_consume_pending_event)
DEFINE_CONDITION_ZERO(
  front_end_has_pending_race_result_condition_adapter,
  front_end_has_pending_race_result)
DEFINE_CONDITION_ZERO(
  front_end_is_alternate_game_mode_condition_adapter,
  front_end_is_alternate_game_mode)
DEFINE_CONDITION_ZERO(
  front_end_is_player_configuration_valid_condition_adapter,
  front_end_is_player_configuration_valid)
DEFINE_CONDITION_ZERO(
  front_end_is_profile_slot_available_condition_adapter,
  front_end_is_profile_slot_available)
DEFINE_CONDITION_ONE(
  front_end_is_selected_rider_style_condition_adapter,
  front_end_is_selected_rider_style)
DEFINE_CONDITION_ZERO(
  front_end_is_single_player_condition_adapter,
  front_end_is_single_player)
DEFINE_CONDITION_ZERO(
  front_end_is_special_mode_enabled_condition_adapter,
  front_end_is_special_mode_enabled)
DEFINE_CONDITION_ZERO(
  front_end_prepare_selected_opponents_condition_adapter,
  front_end_prepare_selected_opponents)
DEFINE_CONDITION_ONE(
  front_end_set_player_descriptor_selection_condition_adapter,
  front_end_set_player_descriptor_selection)
DEFINE_CONDITION_ONE(
  front_end_set_selected_finish_position_condition_adapter,
  front_end_set_selected_finish_position)

#undef DEFINE_COMMAND_VOID_ZERO
#undef DEFINE_COMMAND_VOID_ONE
#undef DEFINE_COMMAND_INT_ZERO
#undef DEFINE_CONDITION_ZERO
#undef DEFINE_CONDITION_ONE
#undef DEFINE_CONDITION_TWO
