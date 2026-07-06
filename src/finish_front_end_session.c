#include "audio.h"
#include "stdlib.h"
/* Reconstructed from the original function at 0x000094E8. */



#include "division_helpers.h"
#include "front_end_commands.h"
#include "rw_semantic_data.h"

#define FRONT_END_RANDOM_STREAM_COUNT 6
#define FRONT_END_EXIT_STREAM_FIRST 0x19
#define FRONT_END_MOVIE_PLAYBACK_FLAGS 2
#define RANDOM_RIDER_TYPE_MASK 7
#define RANDOM_VARIANT_BIT 2
#define RIDER_BACKDROP_AXLE 0x170
#define RIDER_BACKDROP_MIKE 0x337
#define RIDER_BACKDROP_CYDNEY 0x1F1
#define RIDER_BACKDROP_JIM 0x2F6
#define RIDER_BACKDROP_PEARL 0x274
#define RIDER_BACKDROP_JON 0x232
#define RIDER_BACKDROP_RHONDA 0x2B5
#define RIDER_BACKDROP_BOSE 0x1B0
#define FRONT_END_CEL_MODE_OVERLAY 0x0F
#define FRONT_END_CEL_COMMON_OVERLAY 0x99
#define OUTCOME_MODE_STREAM_GROUP_A 2
#define OUTCOME_MODE_STREAM_GROUP_B 3
#define OUTCOME_MODE_STREAM_GROUP_C 4
#define OUTCOME_MODE_STREAM_GROUP_D 5
#define OUTCOME_MODE_LEVEL_STREAM 0x7E
#define OUTCOME_STREAM_GROUP_A_FIRST 3
#define OUTCOME_STREAM_GROUP_B_FIRST 9
#define OUTCOME_STREAM_GROUP_C_FIRST 0x13
#define OUTCOME_STREAM_GROUP_D_FIRST 0x1F
#define OUTCOME_LEVEL_STREAM_FIRST 0x25

int
stop_background_audio_stream(void);
int
stop_front_end_menu_audio(void);
int
initialize_movie_subscribers(void);
int
play_movie_stream(const char *stream_path,
                  int         playback_flags);
int
dispose_movie_subscribers(void);
void
clear_movie_playback_active(void);
void
queue_fullscreen_clear(int unused);
void
present_display_frame(int wait_for_vbl);
int
draw_front_end_cel(int resource_id);
int
finalize_front_end_cel_resource(int resource_id,
                                int seek_resource);
int
RSRC_DisposeFileResources(void *resource_file);
int
dispose_front_end_assets(void);

int
finish_front_end_session(void)
{
  int base;
  int choice;
  int variant;
  int result;
  UnsignedDivisionResult divmod;

  variant = 0;
  stop_background_audio_stream();
  stop_front_end_menu_audio();
  if(background_audio_globals.stream_state != 0 ||
     front_end_asset_state.menu_audio_item != -1)
    {
      return -1;
    }

  if(runtime_feature_flags.streaming_audio_enabled.value != 0 &&
     initialize_movie_subscribers() == 0)
    {
      divmod = unsigned_divide_with_remainder(
        FRONT_END_RANDOM_STREAM_COUNT, (unsigned int)rand());
      play_movie_stream(
        front_end_stream_paths[divmod.remainder +
                               FRONT_END_EXIT_STREAM_FIRST],
        FRONT_END_MOVIE_PLAYBACK_FLAGS);
      result = dispose_movie_subscribers();
      if(result < 0)
        {
          return result;
        }
      clear_movie_playback_active();
    }

  queue_fullscreen_clear(0);
  present_display_frame(1);
  queue_fullscreen_clear(0);
  present_display_frame(1);

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_STANDARD)
    {
      choice = player_profiles[race_mode_state.active_player_index].rider_type;
    }
  else
    {
      choice = GetAudioTime() & RANDOM_RIDER_TYPE_MASK;
      variant = GetAudioTime();
      variant &= ((GetAudioTime() & RANDOM_VARIANT_BIT) >> 1) + 1;
    }

  base = RIDER_BACKDROP_AXLE;
  switch(choice)
    {
    case RIDER_TYPE_MIKE:
      base = RIDER_BACKDROP_MIKE;
      break;
    case RIDER_TYPE_CYDNEY:
      base = RIDER_BACKDROP_CYDNEY;
      break;
    case RIDER_TYPE_JIM:
      base = RIDER_BACKDROP_JIM;
      break;
    case RIDER_TYPE_PEARL:
      base = RIDER_BACKDROP_PEARL;
      break;
    case RIDER_TYPE_JON:
      base = RIDER_BACKDROP_JON;
      break;
    case RIDER_TYPE_RHONDA:
      base = RIDER_BACKDROP_RHONDA;
      break;
    case RIDER_TYPE_BOSE:
      base = RIDER_BACKDROP_BOSE;
      break;
    case RIDER_TYPE_AXLE:
      base = RIDER_BACKDROP_AXLE;
      break;
    }

  base += variant;
  queue_fullscreen_clear(0);
  draw_front_end_cel(base);
  /* Keep queued cel source data pinned through submission. */

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_ALTERNATE)
    {
      draw_front_end_cel(FRONT_END_CEL_MODE_OVERLAY);
    }
  draw_front_end_cel(FRONT_END_CEL_COMMON_OVERLAY);
  present_display_frame(1);
  finalize_front_end_cel_resource(base, 1);

  if(race_mode_state.game_mode == FRONT_END_GAME_MODE_ALTERNATE)
    {
      finalize_front_end_cel_resource(FRONT_END_CEL_MODE_OVERLAY, 1);
    }
  finalize_front_end_cel_resource(FRONT_END_CEL_COMMON_OVERLAY, 1);

  result = dispose_front_end_assets();
  if(result < 0)
    {
      return result;
    }
  set_text_shadow_offset(0, 0);
  race_mode_state.outcome_mode = 1;
  return 0;
}


void
front_end_load_mode_backdrop(void)
{
  int stream_index;
  UnsignedDivisionResult divmod;
  UnsignedDivisionResult div10;

  if(front_end_preview_state.operation_busy != 0)
    {
      return;
    }

  stream_index = race_mode_state.outcome_mode;
  if(stream_index == OUTCOME_MODE_STREAM_GROUP_C)
    {
      divmod = unsigned_divide_with_remainder(
        FRONT_END_RANDOM_STREAM_COUNT, (unsigned int)rand());
      stream_index = divmod.remainder + OUTCOME_STREAM_GROUP_C_FIRST;
    }
  else if(stream_index < OUTCOME_MODE_STREAM_GROUP_D)
    {
      if(stream_index == OUTCOME_MODE_STREAM_GROUP_A)
        {
          divmod = unsigned_divide_with_remainder(
            FRONT_END_RANDOM_STREAM_COUNT, (unsigned int)rand());
          stream_index = divmod.remainder + OUTCOME_STREAM_GROUP_A_FIRST;
        }
      else if(stream_index == OUTCOME_MODE_STREAM_GROUP_B)
        {
          div10 = unsigned_divide_by_ten_with_remainder(
            (unsigned int)rand());
          stream_index = div10.remainder + OUTCOME_STREAM_GROUP_B_FIRST;
        }
      else
        {
          return;
        }
    }
  else if(stream_index == OUTCOME_MODE_STREAM_GROUP_D)
    {
      divmod = unsigned_divide_with_remainder(
        FRONT_END_RANDOM_STREAM_COUNT, (unsigned int)rand());
      stream_index = divmod.remainder + OUTCOME_STREAM_GROUP_D_FIRST;
    }
  else
    {
      if(stream_index != OUTCOME_MODE_LEVEL_STREAM)
        {
          return;
        }
      stream_index =
        player_profiles[race_mode_state.active_player_index].level +
        OUTCOME_LEVEL_STREAM_FIRST;
    }

  if(stream_index < 0)
    {
      return;
    }
  if(runtime_feature_flags.streaming_audio_enabled.value == 0)
    {
      return;
    }

  stop_front_end_menu_audio();
  stop_background_audio_stream();
  if(background_audio_globals.stream_state == 0 &&
     front_end_asset_state.menu_audio_item == -1 &&
     initialize_movie_subscribers() == 0)
    {
      play_movie_stream(front_end_stream_paths[stream_index],
                        FRONT_END_MOVIE_PLAYBACK_FLAGS);
      dispose_movie_subscribers();
    }
}
