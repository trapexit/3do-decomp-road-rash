#include "stdio.h"
#include "stdlib.h"
#include "string.h"
/* Reconstructed from the original function at 0x0000A1C8 using Ghidra and assembly. */



#include "bss_early_data.h"
#include "division_helpers.h"
#include "front_end_cel_runtime.h"
#include "racer_runtime.h"
#include "rw_semantic_data.h"
#include "text_render_runtime.h"

#define FRONT_END_DRAW_ITEM (display_runtime_globals.screen_items[0])

enum RiderSelectionConstant
{
  RIDER_TYPE_FIRST = 1,
  RIDER_TYPE_LAST = 8,
  RIDER_TYPE_RANDOM_MASK = 7,
  RIDER_STYLE_LAST = 14,
  SELECTED_OPPONENT_COUNT = 4,
  OPPONENT_CHOICE_CAPACITY = 10,
  OPPONENT_PROGRESS_ROW_COUNT = 10,
  OPPONENT_PROGRESS_WORDS_PER_ROW = 3,
  OPPONENT_RESOURCE_VARIANT_COUNT = 3,
  OPPONENT_FALLBACK_RESOURCE_FIRST = 0x10,
  OPPONENT_SELECTION_RETRY_LIMIT = 5000,
  OPPONENT_EASY_METRIC_BASE = 16000,
  OPPONENT_EASY_LEVEL_SCALE = 1500,
  OPPONENT_EASY_RANK_SCALE = 1000,
  OPPONENT_MEDIUM_METRIC_BASE = 32000,
  OPPONENT_MEDIUM_LEVEL_SCALE = 3000,
  OPPONENT_MEDIUM_RANK_SCALE = 2000,
  OPPONENT_CATEGORY_EASY = 1,
  OPPONENT_CATEGORY_MEDIUM = 2,
  OPPONENT_CATEGORY_HARD = 3,
  MAXIMUM_SELECTION_LEVEL = 4,
  MAXIMUM_FRONT_END_LEVEL = 5,
  MINIMUM_FULL_PROGRESS_RANK = 4
};

enum RiderStyleId
{
  RIDER_STYLE_SPORT_450 = 0,
  RIDER_STYLE_SWALLOW = 1,
  RIDER_STYLE_ZYX_250 = 2,
  RIDER_STYLE_KILLER = 3,
  RIDER_STYLE_PICO = 4,
  RIDER_STYLE_711_SS1 = 5,
  RIDER_STYLE_RAVEN_N = 6,
  RIDER_STYLE_VIPERA_N = 7,
  RIDER_STYLE_ZYX_750N = 8,
  RIDER_STYLE_ASSASSINO = 9,
  RIDER_STYLE_VAMPIRO = 10,
  RIDER_STYLE_M10 = 11,
  RIDER_STYLE_ZYX_750 = 12,
  RIDER_STYLE_GRANDE = 13,
  RIDER_STYLE_AGGRESSORE = 14
};

enum RiderTypeId
{
  RIDER_TYPE_MIKE = 1,
  RIDER_TYPE_CYDNEY = 2,
  RIDER_TYPE_JIM = 3,
  RIDER_TYPE_PEARL = 4,
  RIDER_TYPE_JON = 5,
  RIDER_TYPE_RHONDA = 6,
  RIDER_TYPE_BOSE = 7,
  RIDER_TYPE_AXLE = 8
};

enum FrontEndRaceMode
{
  FRONT_END_GAME_MODE_STANDARD = 0,
  FRONT_END_GAME_MODE_ALTERNATE = 1,
  SINGLE_PLAYER_COUNT = 1,
  RACE_OUTCOME_MODE_PODIUM = 2,
  RACE_OUTCOME_MODE_NON_PODIUM = 3,
  RACE_OUTCOME_MODE_BIKE_REPAIR = 4,
  RACE_OUTCOME_MODE_FORFEIT = 5,
  RACE_OUTCOME_MODE_ADVANCE_LEVEL = 0x7E
};

enum FrontEndTextConstant
{
  FRONT_END_TEXT_LAYER = 2,
  FRONT_END_COLOR_NORMAL = 0xBF,
  FRONT_END_COLOR_MUTED = 0xC6,
  FRONT_END_COLOR_HIGHLIGHT_RED = 0x28,
  FRONT_END_COLOR_REWARD_RED = 0xF7,
  FRONT_END_COLOR_REWARD_GREEN = 0xD6,
  FRONT_END_COLOR_REWARD_BLUE = 0x10,
  BALANCE_TEXT_CAPACITY = 12,
  RIDER_SUMMARY_NAME_CAPACITY = 12,
  RIDER_SUMMARY_TEXT_CAPACITY = 60,
  RIDER_SUMMARY_NAME_BYTES = 9,
  RIDER_NAME_FALLBACK_INDEX = 9,
  BIKE_SUMMARY_RIDER_CAPACITY = 8,
  BIKE_SUMMARY_RIDER_BYTES = 7,
  BIKE_SUMMARY_NAME_CAPACITY = 20,
  BIKE_SUMMARY_NAME_BYTES = 0x11,
  BIKE_SUMMARY_TEXT_CAPACITY = 48,
  STYLE_PRICE_TEXT_CAPACITY = 48,
  AFFORDABILITY_TEXT_CAPACITY = 8,
  BIKE_STAT_TEXT_CAPACITY = 20,
  UPGRADE_COST_TEXT_CAPACITY = 32
};

enum FrontEndDisplayPosition
{
  BALANCE_TEXT_X = 0x4A,
  BALANCE_TEXT_Y = 0x8C,
  RIDER_SUMMARY_TEXT_X = 0x46,
  RIDER_SUMMARY_TEXT_Y = 0xA6,
  BIKE_SUMMARY_TEXT_X = 0x18,
  BIKE_SUMMARY_TEXT_Y = 0xA6,
  STYLE_PRICE_TEXT_X = 0x18,
  STYLE_PRICE_TEXT_Y = 0x94,
  AFFORDABILITY_TEXT_BASELINE_OFFSET = 2,
  BIKE_STAT_TEXT_X_OFFSET = -2,
  BIKE_STAT_TEXT_Y_OFFSET = 2,
  UPGRADE_COST_TEXT_X = 0xED,
  UPGRADE_COST_TEXT_Y = 0xC5
};

enum FrontEndAffordabilityPosition
{
  SPORT_450_PRICE_X = 0xF4,
  SPORT_450_PRICE_Y = 0x53,
  SWALLOW_PRICE_X = 0x28,
  SWALLOW_PRICE_Y = 0x5A,
  ZYX_250_PRICE_X = 0xF4,
  ZYX_250_PRICE_Y = 0x5A,
  KILLER_PRICE_X = 0x2B,
  KILLER_PRICE_Y = 0x62,
  PICO_PRICE_X = 0xF4,
  PICO_PRICE_Y = 0x5A,
  SS1_PRICE_X = 0x2B,
  SS1_PRICE_Y = 0x5A,
  RAVEN_N_PRICE_X = 0xF0,
  RAVEN_N_PRICE_Y = 0x5B,
  VIPERA_N_PRICE_X = 0xE8,
  VIPERA_N_PRICE_Y = 0x5C,
  ZYX_750N_PRICE_X = 0xF5,
  ZYX_750N_PRICE_Y = 0x57,
  ASSASSINO_PRICE_X = 0xEA,
  ASSASSINO_PRICE_Y = 0x59,
  VAMPIRO_PRICE_X = 0x2A,
  VAMPIRO_PRICE_Y = 0x62,
  M10_PRICE_X = 0xF0,
  M10_PRICE_Y = 0x55,
  ZYX_750_PRICE_X = 0x2D,
  ZYX_750_PRICE_Y = 0x59,
  GRANDE_PRICE_X = 0x2B,
  GRANDE_PRICE_Y = 0x59,
  AGGRESSORE_PRICE_X = 0xF4,
  AGGRESSORE_PRICE_Y = 0x5A
};

enum FrontEndBikeStatisticPosition
{
  DEFAULT_BIKE_TITLE_X = 0xC2,
  DEFAULT_BIKE_TITLE_Y = 0x5A,
  DEFAULT_BIKE_PRICE_Y = 0x69,
  MIKE_BIKE_TITLE_X = 0x3D,
  CYDNEY_BIKE_TITLE_X = 0xB3,
  JIM_BIKE_TITLE_X = 0x3A,
  PEARL_BIKE_TITLE_X = 0x3D,
  JON_BIKE_TITLE_X = 0xC6,
  JON_BIKE_TITLE_Y = 0x5D,
  JON_BIKE_PRICE_Y = 0x6B,
  RHONDA_BIKE_TITLE_X = 0x3D,
  BOSE_BIKE_TITLE_X = 0xBD
};

enum FrontEndProgressionConstant
{
  FIRST_UNLOCKED_LEVEL = 1,
  SECOND_UNLOCKED_LEVEL = 2,
  THIRD_UNLOCKED_LEVEL = 3,
  FOURTH_UNLOCKED_LEVEL = 4,
  COMPLETION_LEVEL = 5,
  DISPLAYED_RESULT_COUNT = 3,
  LAST_DISPLAYED_RESULT_INDEX = 3,
  LAST_PODIUM_FINISH_INDEX = 2,
  LEVEL_UPGRADE_COST = 400
};

void
toggle_active_front_end_player(void);
void
reset_player_profiles(int reset_first,
                      int reset_second,
                      int preserve_selection);
int
draw_front_end_cel(int resource_id);
void
start_front_end_menu_audio(void);
void
stop_front_end_menu_audio(void);
void
draw_race_result_row(int rank_index);
int
play_front_end_completion_animation(void);
int
submit_cel_batch(void);
void
start_next_background_audio_stream(void);
void
stop_background_audio_stream(void);
static
RiderProfile *
current_front_end_rider(void)
{
  return &player_profiles[race_mode_state.active_player_index];
}


static
int
choose_random_rider_type(void)
{
  int value;
  int negative;

  value = rand();
  negative = value < 0;
  if(negative)
    {
      value = -value;
    }
  value &= RIDER_TYPE_RANDOM_MASK;
  if(negative)
    {
      value = -value;
    }
  return value + 1;
}


static
int
player_race_rank_penalty(void)
{
  RacerEntity *player;
  int value;

  player = (RacerEntity *)race_rider_state.player_object;
  if(player == 0)
    {
      return 0;
    }

  value = player->paired_state->race_rank;
  if(value < MINIMUM_FULL_PROGRESS_RANK)
    {
      return MINIMUM_FULL_PROGRESS_RANK - value;
    }
  return 0;
}


static
int
opponent_progress_metric(OpponentProgressionRecord *records,
                         int                        rider_type,
                         int                        row)
{
  int *values;

  if(row >= OPPONENT_PROGRESS_ROW_COUNT)
    {
      return 0;
    }
  values = (int *)&records[rider_type]
           .words[row * OPPONENT_PROGRESS_WORDS_PER_ROW];
  return values[OPPONENT_CATEGORY_EASY] +
         values[OPPONENT_CATEGORY_MEDIUM] - values[0];
}


void
select_rider_opponents(int player_index)
{
  signed char selected_types[SELECTED_OPPONENT_COUNT];
  int choices[OPPONENT_CHOICE_CAPACITY];
  RiderProfile *rider;
  OpponentChoiceLevel *level_data;
  OpponentChoiceRecord *choice_record;
  int index;
  int rider_type;
  int level;
  int candidate;
  int retries;
  int duplicate;
  int compare_index;
  int row;
  int metric;
  int player_level;
  int penalty;
  int category;
  int style;
  int choice_count;
  OpponentProgressionRecord *player_records;
  OpponentProgressionRecord *current_type_record;
  SignedDivisionResult divmod;

  index = (signed char)player_index;
  rider = &player_profiles[index];
  rider_type = rider->rider_type;
  level = rider->level;
  if(level > MAXIMUM_SELECTION_LEVEL)
    {
      level = MAXIMUM_SELECTION_LEVEL;
    }

  if(rider_type < RIDER_TYPE_FIRST || rider_type > RIDER_TYPE_LAST)
    {
      for(candidate = 0; candidate < SELECTED_OPPONENT_COUNT;
          candidate++)
        {
          rider->opponent_rider_types[candidate] = 0;
          rider->opponent_resource_ids[candidate] =
            candidate + OPPONENT_FALLBACK_RESOURCE_FIRST;
          rider->opponent_styles[candidate] = RIDER_STYLE_SWALLOW;
        }
      return;
    }

  retries = OPPONENT_SELECTION_RETRY_LIMIT;
  for(candidate = 0; candidate < SELECTED_OPPONENT_COUNT; candidate++)
    {
      for(;;)
        {
          selected_types[candidate] =
            (signed char)choose_random_rider_type();
          if(selected_types[candidate] == rider_type)
            {
              continue;
            }

          retries--;
          duplicate = 0;
          for(compare_index = 0; compare_index < candidate;
              compare_index++)
            {
              if(selected_types[candidate] ==
                 selected_types[compare_index])
                {
                  duplicate = 1;
                  break;
                }
            }
          if(!duplicate || retries <= 0)
            {
              break;
            }
        }
    }

  player_records = gRiderProgressionProfiles[index];
  current_type_record = &player_records[rider_type];
  level_data = &opponent_choice_levels[level];

  for(index = 0; index < SELECTED_OPPONENT_COUNT; index++)
    {
      candidate = selected_types[index];
      row = (signed char)current_type_record->fields.current_stage;
      metric = opponent_progress_metric(player_records, candidate, row);
      player_level =
        player_profiles[race_mode_state.active_player_index].level;
      penalty = player_race_rank_penalty();

      if(metric < OPPONENT_EASY_METRIC_BASE -
         player_level * OPPONENT_EASY_LEVEL_SCALE -
         penalty * OPPONENT_EASY_RANK_SCALE)
        {
          category = OPPONENT_CATEGORY_EASY;
        }
      else
        {
          metric = opponent_progress_metric(player_records, candidate, row);
          penalty = player_race_rank_penalty();
          if(metric < OPPONENT_MEDIUM_METRIC_BASE -
             player_level * OPPONENT_MEDIUM_LEVEL_SCALE -
             penalty * OPPONENT_MEDIUM_RANK_SCALE)
            {
              category = OPPONENT_CATEGORY_MEDIUM;
            }
          else
            {
              category = OPPONENT_CATEGORY_HARD;
            }
        }

      style = opponent_choice_style_map[category];
      choice_record = &level_data->riders[candidate - 1];
      choice_count = 0;
      if(choice_record->preferred_rider_types[style][0] == rider_type)
        {
          choices[choice_count++] =
            choice_record
            ->resource_choices[style + OPPONENT_RESOURCE_VARIANT_COUNT]
            [0];
        }
      if(choice_record->preferred_rider_types[style][1] == rider_type)
        {
          choices[choice_count++] =
            choice_record
            ->resource_choices[style + OPPONENT_RESOURCE_VARIANT_COUNT]
            [1];
        }
      choices[choice_count++] = choice_record->resource_choices[style][0];
      choices[choice_count++] = choice_record->resource_choices[style][1];

      divmod = signed_divide_with_remainder((unsigned int)choice_count,
                                            (unsigned int)rand());
      rider->opponent_rider_types[index] = candidate;
      rider->opponent_resource_ids[index] = choices[divmod.remainder];
      rider->opponent_styles[index] = style;
    }
}


void
front_end_preload_selected_opponent(int slot)
{
  RiderProfile *rider;
  int opponent;
  int resource;
  int variant;

  rider = current_front_end_rider();
  opponent = (signed char)(rider->opponent_rider_types[slot] - 1);
  resource = rider->opponent_resource_ids[slot];
  variant = (signed char)rider->opponent_styles[slot];

  if(opponent < 0)
    {
      draw_front_end_cel(OPPONENT_FALLBACK_RESOURCE_FIRST - 1);
      draw_front_end_cel(OPPONENT_FALLBACK_RESOURCE_FIRST - 1);
    }
  else
    {
      draw_front_end_cel(opponent_portrait_resource_ids[opponent][variant]);
      draw_front_end_cel(opponent_bike_resource_ids[opponent][variant]);
    }
  draw_front_end_cel(resource);
}


int
front_end_has_pending_race_result(void)
{
  return race_mode_state.outcome_mode != 0;
}


int
front_end_is_alternate_game_mode(void)
{
  return race_mode_state.game_mode == FRONT_END_GAME_MODE_ALTERNATE;
}


int
front_end_is_single_player(void)
{
  return race_mode_state.player_count == SINGLE_PLAYER_COUNT;
}


int
front_end_is_special_mode_enabled(void)
{
  return runtime_feature_flags.music_option_enabled.value;
}


int
front_end_consume_pending_event(void)
{
  int pending;

  pending = front_end_preview_state.initial_refresh_pending != 0;
  if(pending)
    {
      front_end_preview_state.initial_refresh_pending = 0;
    }
  return pending;
}


int
front_end_can_afford_rider_style(int style)
{
  RiderProfile *rider;
  int current_style;
  int value;

  rider = current_front_end_rider();
  current_style = rider->rider_style;
  if(current_style == style)
    {
      return 1;
    }

  value = rider->balance + rider_style_resale_values[current_style];
  return value < rider_style_purchase_prices[style];
}


void
front_end_draw_current_balance(void)
{
  char text[BALANCE_TEXT_CAPACITY];

  set_text_color(FRONT_END_COLOR_MUTED, FRONT_END_COLOR_MUTED,
                 FRONT_END_COLOR_MUTED);
  sprintf(text, "%6ld", current_front_end_rider()->balance);
  draw_text(FRONT_END_DRAW_ITEM, BALANCE_TEXT_X, BALANCE_TEXT_Y,
            (int)text, 0, FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


static const char front_end_rider_names_long[10][10] =
{
  "Player 1", "Mike    ", "Cydney  ", "Jim     ", "Pearl   ",
  "Jon     ", "Rhonda  ", "Bose    ", "Axle    ", "Player 2"
};

void
front_end_draw_selected_rider_summary(void)
{
  char name[RIDER_SUMMARY_NAME_CAPACITY];
  char text[RIDER_SUMMARY_TEXT_CAPACITY];
  RiderProfile *rider;
  int type;

  rider = current_front_end_rider();
  type = rider->rider_type;
  if(type < 0 || type > RIDER_TYPE_LAST)
    {
      type = RIDER_NAME_FALLBACK_INDEX;
    }
  memcpy(name, front_end_rider_names_long[type],
         RIDER_SUMMARY_NAME_BYTES);
  set_text_color(FRONT_END_COLOR_NORMAL, FRONT_END_COLOR_NORMAL,
                 FRONT_END_COLOR_NORMAL);

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      sprintf(text, "%9s  %6s%d  %7s%d", name, "Level ",
              rider->level + 1, "Cash: $", rider->balance);
    }
  else
    {
      sprintf(text, "%9s  %6s%d", name, "Level ", rider->level + 1);
    }
  draw_text(FRONT_END_DRAW_ITEM, RIDER_SUMMARY_TEXT_X,
            RIDER_SUMMARY_TEXT_Y, (int)text, 0, FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


static const char front_end_rider_names_short[8][8] =
{
  "Mike  ", "Cydney", "Jim  ", "Pearl ",
  "Jon  ", "Rhonda", "Bose  ", "Axle  "
};

static const char front_end_bike_names[15][18] =
{
  "Bike:Sport 450  ", "Bike:Swallow    ", "Bike:ZYX 250    ",
  "Bike:Killer     ", "Bike:Pico       ", "Bike:7/11 SS1   ",
  "Bike:Raven N    ", "Bike:Vipera N   ", "Bike:ZYX 750N   ",
  "Bike:Assassino  ", "Bike:Vampiro    ", "Bike:M10        ",
  "Bike:ZYX 750    ", "Bike:Grande     ", "Bike:Aggressore "
};

void
front_end_draw_selected_bike_summary(void)
{
  char rider_name[BIKE_SUMMARY_RIDER_CAPACITY] = "";
  char bike_name[BIKE_SUMMARY_NAME_CAPACITY] = "No Bike";
  char text[BIKE_SUMMARY_TEXT_CAPACITY];
  RiderProfile *rider;
  int type;
  int style;
  int cash;

  rider = current_front_end_rider();
  type = rider->rider_type;
  style = rider->rider_style;
  if(type >= RIDER_TYPE_FIRST && type <= RIDER_TYPE_LAST)
    {
      memcpy(rider_name, front_end_rider_names_short[type - 1],
             BIKE_SUMMARY_RIDER_BYTES);
    }
  if(style >= 0 && style <= RIDER_STYLE_LAST)
    {
      memcpy(bike_name, front_end_bike_names[style],
             BIKE_SUMMARY_NAME_BYTES);
    }

  set_text_color(FRONT_END_COLOR_NORMAL, FRONT_END_COLOR_NORMAL,
                 FRONT_END_COLOR_NORMAL);
  cash = rider->balance + rider_style_resale_values[style];
  sprintf(text, "%7s %12s%d%17s", rider_name,
          "Cash+Trade:$", cash, bike_name);
  draw_text(FRONT_END_DRAW_ITEM, BIKE_SUMMARY_TEXT_X,
            BIKE_SUMMARY_TEXT_Y, (int)text, 0, FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


void
front_end_draw_rider_style_price(int style)
{
  char text[STYLE_PRICE_TEXT_CAPACITY];

  set_text_color(FRONT_END_COLOR_NORMAL, FRONT_END_COLOR_NORMAL,
                 FRONT_END_COLOR_NORMAL);
  sprintf(text, "%9s%d", "Price: $", rider_style_purchase_prices[style]);
  draw_text(FRONT_END_DRAW_ITEM, STYLE_PRICE_TEXT_X, STYLE_PRICE_TEXT_Y,
            (int)text, 0, FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


void
front_end_draw_rider_style_affordability(int style)
{
  char text[AFFORDABILITY_TEXT_CAPACITY];
  RiderProfile *rider;
  int x;
  int y;
  int buying_power;

  x = AGGRESSORE_PRICE_X;
  y = AGGRESSORE_PRICE_Y;
  switch(style)
    {
    case RIDER_STYLE_SPORT_450:
      x = SPORT_450_PRICE_X;
      y = SPORT_450_PRICE_Y;
      break;
    case RIDER_STYLE_SWALLOW:
      x = SWALLOW_PRICE_X;
      y = SWALLOW_PRICE_Y;
      break;
    case RIDER_STYLE_ZYX_250:
      x = ZYX_250_PRICE_X;
      y = ZYX_250_PRICE_Y;
      break;
    case RIDER_STYLE_KILLER:
      x = KILLER_PRICE_X;
      y = KILLER_PRICE_Y;
      break;
    case RIDER_STYLE_PICO:
      x = PICO_PRICE_X;
      y = PICO_PRICE_Y;
      break;
    case RIDER_STYLE_711_SS1:
      x = SS1_PRICE_X;
      y = SS1_PRICE_Y;
      break;
    case RIDER_STYLE_RAVEN_N:
      x = RAVEN_N_PRICE_X;
      y = RAVEN_N_PRICE_Y;
      break;
    case RIDER_STYLE_VIPERA_N:
      x = VIPERA_N_PRICE_X;
      y = VIPERA_N_PRICE_Y;
      break;
    case RIDER_STYLE_ZYX_750N:
      x = ZYX_750N_PRICE_X;
      y = ZYX_750N_PRICE_Y;
      break;
    case RIDER_STYLE_ASSASSINO:
      x = ASSASSINO_PRICE_X;
      y = ASSASSINO_PRICE_Y;
      break;
    case RIDER_STYLE_VAMPIRO:
      x = VAMPIRO_PRICE_X;
      y = VAMPIRO_PRICE_Y;
      break;
    case RIDER_STYLE_M10:
      x = M10_PRICE_X;
      y = M10_PRICE_Y;
      break;
    case RIDER_STYLE_ZYX_750:
      x = ZYX_750_PRICE_X;
      y = ZYX_750_PRICE_Y;
      break;
    case RIDER_STYLE_GRANDE:
      x = GRANDE_PRICE_X;
      y = GRANDE_PRICE_Y;
      break;
    case RIDER_STYLE_AGGRESSORE:
      x = AGGRESSORE_PRICE_X;
      y = AGGRESSORE_PRICE_Y;
      break;
    }

  rider = current_front_end_rider();
  buying_power =
    rider->balance + rider_style_resale_values[rider->rider_style];
  if(buying_power < rider_style_purchase_prices[style])
    {
      set_text_color(FRONT_END_COLOR_NORMAL, FRONT_END_COLOR_NORMAL,
                     FRONT_END_COLOR_NORMAL);
    }
  else
    {
      set_text_color(FRONT_END_COLOR_NORMAL,
                     FRONT_END_COLOR_HIGHLIGHT_RED,
                     FRONT_END_COLOR_HIGHLIGHT_RED);
    }

  sprintf(text, "%1s%d", "$", rider_style_purchase_prices[style]);
  draw_text(FRONT_END_DRAW_ITEM, x,
            y + AFFORDABILITY_TEXT_BASELINE_OFFSET, (int)text, 0,
            FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


void
front_end_draw_rider_bike_statistics(int type)
{
  char title[BIKE_STAT_TEXT_CAPACITY];
  char text[BIKE_STAT_TEXT_CAPACITY];
  const char *title_text;
  int x;
  int title_y;
  int price_y;

  if(type < RIDER_TYPE_FIRST || type > RIDER_TYPE_LAST)
    {
      type = RIDER_TYPE_AXLE;
    }

  title_text = "Corsair Swallow ";
  x = DEFAULT_BIKE_TITLE_X;
  title_y = DEFAULT_BIKE_TITLE_Y;
  price_y = DEFAULT_BIKE_PRICE_Y;
  switch(type)
    {
    case RIDER_TYPE_MIKE:
      x = MIKE_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_CYDNEY:
      title_text = "Kamikaze ZYX 250";
      x = CYDNEY_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_JIM:
      title_text = "Perro Pico      ";
      x = JIM_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_PEARL:
      title_text = "Perro Pico      ";
      x = PEARL_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_JON:
      x = JON_BIKE_TITLE_X;
      title_y = JON_BIKE_TITLE_Y;
      price_y = JON_BIKE_PRICE_Y;
      break;
    case RIDER_TYPE_RHONDA:
      x = RHONDA_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_BOSE:
      title_text = "Kamikaze ZYX 250";
      x = BOSE_BIKE_TITLE_X;
      break;
    case RIDER_TYPE_AXLE:
      break;
    }

  memset(title, 0, sizeof(title));
  strcpy(title, title_text);
  set_text_color(FRONT_END_COLOR_NORMAL, FRONT_END_COLOR_NORMAL,
                 FRONT_END_COLOR_NORMAL);
  sprintf(text, "%17s", title);
  draw_text(FRONT_END_DRAW_ITEM, x + BIKE_STAT_TEXT_X_OFFSET,
            title_y + BIKE_STAT_TEXT_Y_OFFSET, (int)text, 0,
            FRONT_END_TEXT_LAYER);

  sprintf(text, "%1s%d", "$", rider_starting_balances[type]);
  draw_text(FRONT_END_DRAW_ITEM, x,
            price_y + BIKE_STAT_TEXT_Y_OFFSET, (int)text, 0,
            FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


int
front_end_is_selected_rider_style(int style)
{
  return current_front_end_rider()->rider_style == style;
}


int
front_end_prepare_selected_opponents(void)
{
  select_rider_opponents(race_mode_state.active_player_index);
  if(race_mode_state.outcome_mode != RACE_OUTCOME_MODE_ADVANCE_LEVEL)
    {
      toggle_active_front_end_player();
    }
  return race_mode_state.outcome_mode == RACE_OUTCOME_MODE_ADVANCE_LEVEL;
}


void
front_end_show_selected_level(void)
{
  RiderProfile *rider;

  rider = current_front_end_rider();
  switch(rider->level)
    {
    case FIRST_UNLOCKED_LEVEL:
      apply_front_end_cel_descriptors(front_end_cel_group_295);
      break;
    case SECOND_UNLOCKED_LEVEL:
      apply_front_end_cel_descriptors(front_end_cel_group_296);
      break;
    case THIRD_UNLOCKED_LEVEL:
      apply_front_end_cel_descriptors(front_end_cel_group_297);
      break;
    case FOURTH_UNLOCKED_LEVEL:
      apply_front_end_cel_descriptors(front_end_cel_group_298);
      break;
    case COMPLETION_LEVEL:
      play_front_end_completion_animation();
      rider->level = FOURTH_UNLOCKED_LEVEL;
      break;
    }
  toggle_active_front_end_player();
}


void
front_end_set_selected_level(int level)
{
  RiderProfile *rider;

  rider = current_front_end_rider();
  rider->level = (signed char)level;
  rider->completed_courses = 0;
}


int
front_end_set_selected_finish_position(int value)
{
  race_mode_state.background_audio_index = (signed char)value;
  stop_front_end_menu_audio();
  stop_background_audio_stream();
  start_next_background_audio_stream();
  start_front_end_menu_audio();
  return 1;
}


static
void
reset_missing_rider(int index)
{
  reset_player_profiles(index == 0, index == 1, 0);
}


static
void
draw_upgrade_cost(int amount)
{
  char text[UPGRADE_COST_TEXT_CAPACITY];

  set_text_color(FRONT_END_COLOR_REWARD_RED,
                 FRONT_END_COLOR_REWARD_GREEN,
                 FRONT_END_COLOR_REWARD_BLUE);
  sprintf(text, "$%ld", amount);
  draw_text(FRONT_END_DRAW_ITEM, UPGRADE_COST_TEXT_X,
            UPGRADE_COST_TEXT_Y, (int)text, 0, FRONT_END_TEXT_LAYER);
  submit_cel_batch();
}


void
front_end_update_rider_progression(void)
{
  SignedDivisionResult div10;
  RiderProfile *rider;
  int mode;
  int index;
  int rank;
  int old_level;

  mode = race_mode_state.outcome_mode;
  index = race_mode_state.active_player_index;
  rider = &player_profiles[index];

  if(mode == RACE_OUTCOME_MODE_PODIUM ||
     mode == RACE_OUTCOME_MODE_NON_PODIUM ||
     mode == RACE_OUTCOME_MODE_ADVANCE_LEVEL)
    {
      for(rank = 0; rank < DISPLAYED_RESULT_COUNT; rank++)
        {
          draw_race_result_row(rank);
        }
      if(race_mode_state.finish_position > LAST_PODIUM_FINISH_INDEX)
        {
          race_results[LAST_DISPLAYED_RESULT_INDEX].rider_name_index =
            rider->rider_type;
          draw_race_result_row(race_mode_state.finish_position);
        }

      if(mode == RACE_OUTCOME_MODE_ADVANCE_LEVEL &&
         front_end_preview_state.operation_busy == 0)
        {
          old_level = rider->level;
          rider->level = (signed char)(old_level + 1);
          if(old_level > MAXIMUM_FRONT_END_LEVEL)
            {
              rider->level = MAXIMUM_FRONT_END_LEVEL;
            }
          select_rider_opponents(index);
        }
    }
  else if(mode == RACE_OUTCOME_MODE_BIKE_REPAIR &&
          race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      if(rider->balance < 0)
        {
          reset_missing_rider(index);
        }
      else
        {
          draw_upgrade_cost((rider->level + 1) * LEVEL_UPGRADE_COST);
        }
    }
  else if(mode == RACE_OUTCOME_MODE_FORFEIT &&
          race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      if(rider->balance < 0)
        {
          reset_missing_rider(index);
        }
      else
        {
          div10 = signed_divide_by_ten_with_remainder(
            rider_style_purchase_prices[rider->rider_style]);
          draw_upgrade_cost(div10.quotient);
        }
    }

  if(front_end_preview_state.transition_completed != 0 &&
     race_input_latch_state.event_latched == 0)
    {
      reset_player_profiles(1, 1, 1);
      front_end_preview_state.transition_completed = 0;
    }
}
