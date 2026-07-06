#include "stdio.h"

#include "string.h"

#include "platform.h"

#include "application_runtime.h"
#include "audio_system.h"
#include "display_cel_runtime.h"
#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"
#include "memory_manager_runtime.h"
#include "movie_playback_runtime.h"
#include "race_audio_runtime.h"
#include "racer_auxiliary_cache.h"
#include "resource_manager_runtime.h"
#include "rider_visual_resource_binding.h"
#include "road_renderer_runtime.h"
#include "roadside_object_rendering.h"
#include "rw_semantic_data.h"
#include "spatial_audio_event_binding.h"
#include "stream_input_runtime.h"
#include "text_render_runtime.h"

void
initialize_background_audio_selection(void);
int
RSRC_DisposeFileResources(void *resource_file);
void
set_display_vbl_synchronization_enabled(int enabled);
int
run_race_input_and_simulation_loop(void);

enum ApplicationDisplayConfiguration
{
  FRONT_END_CLIP_WIDTH = DISPLAY_WIDTH,
  FRONT_END_CLIP_HEIGHT = DISPLAY_HEIGHT,
  RACE_CLIP_WIDTH = 300,
  RACE_CLIP_HEIGHT = 230,
  TEXT_COLOR_MAX_COMPONENT = 0xFF
};

static const char small_font[] = "Rash/Small 0..9 Font";
static const char large_font[] = "Rash/Large 0..9 Font";
static const char resource_name[] = "Rash3DO.RSRC";
static const char intro_stream[] = "intro/intro.STREAM";
static const char race_start_failure_format[] =
  "Race startup failed: %s\n";
static const char race_audio_failure_message[] =
  "Race audio disabled after initialization failure\n";
static const char race_score_failure_message[] =
  "Race score disabled after initialization failure\n";
static const char course_resource_stage[] = "course resources";
static const char family_resource_stage[] = "family resources";
static const char renderer_worker_stage[] = "renderer worker";
static const char track_simulation_stage[] = "track simulation";
static const char display_runtime_stage[] = "initialize_display_runtime";
static const char text_manager_stage[] = "initialize_text_render_manager";
static const char text_font_stage[] = "load_text_font";
static const char front_end_resource_stage[] = "RSRC_OpenFile";

static
int
report_failed_application_start(const char *failed_stage,
                                int status,
                                const char *detail)
{
  show_application_error(failed_stage, status, detail);
  return 1;
}

static
int
discard_failed_race_start(const char *failed_stage,
                          int family_initialized,
                          int renderer_initialized,
                          int race_audio_initialized)
{
  int result;
  printf(race_start_failure_format, failed_stage);
  if(renderer_initialized)
    {
      stop_road_renderer_worker();
    }
  shutdown_track_simulation();
  result = shutdown_family_resource_runtime();
  if(result < 0)
    {
      return result;
    }
  (void)family_initialized;
  if(race_audio_initialized)
    {
      result = unload_score();
      if(result < 0)
        {
          return result;
        }
      result = shutdown_race_audio();
      if(result < 0)
        {
          return result;
        }
    }
  unload_course_resources();
  unload_rash_options_resources();
  return 0;
}


int
main(void)
{
  char path[FAMILY_RESOURCE_PATH_BYTES];
  FrontEndCelResourceInfo resource_info;
  MemoryHandle *resource;
  SegmentTopologyNode *session;
  int cache_index;
  int family_initialized;
  int race_audio_initialized;
  int renderer_initialized;
  int race_result;
  int result;

  road_renderer_state.startup_render_mode = 0;

  result = initialize_application_runtime();
  if(result != 0)
    {
      return report_failed_application_start(
        "initialize_application_runtime", result, 0);
    }

  if(!initialize_display_runtime())
    {
      return report_failed_application_start(display_runtime_stage, -1, 0);
    }

  if(!initialize_text_render_manager())
    {
      return report_failed_application_start(text_manager_stage, -1, 0);
    }

  resource_catalog_handles.small_digit_font =
    load_text_font(small_font);
  if(resource_catalog_handles.small_digit_font == 0)
    {
      return report_failed_application_start(text_font_stage, -1, small_font);
    }

  resource_catalog_handles.large_digit_font =
    load_text_font(large_font);
  if(resource_catalog_handles.large_digit_font == 0)
    {
      return report_failed_application_start(text_font_stage, -1, large_font);
    }

  configure_music_option_for_available_memory();
  initialize_stream_input_state();

  if(runtime_feature_flags.game_audio_enabled.value == 0)
    {
      runtime_feature_flags.background_audio_enabled.value = 0;
    }
  if(runtime_feature_flags.game_audio_enabled.value != 0 ||
     runtime_feature_flags.streaming_audio_enabled.value != 0)
    {
      result = initialize_audio_system();
      if(result != 0)
        {
          return report_failed_application_start(
            "initialize_audio_system", result, 0);
        }
      runtime_feature_flags.game_audio_enabled.value =
        (unsigned char)(runtime_feature_flags.game_audio_enabled.value != 0);
      runtime_feature_flags.streaming_audio_enabled.value =
        (unsigned char)(runtime_feature_flags.streaming_audio_enabled.value != 0);
      result = initialize_async_stream_message_port(0);
      if(result != 0)
        {
          return report_failed_application_start(
            "initialize_async_stream_message_port", result, 0);
        }
      if(runtime_feature_flags.background_audio_enabled.value != 0)
        {
          initialize_background_audio_selection();
        }
    }

  strcpy(path, resource_root_path);
  strcat(path, resource_name);
  resource = RSRC_OpenFile(path, 0);
  if(resource == 0)
    {
      return report_failed_application_start(front_end_resource_stage, -1, path);
    }

  if(load_front_end_cel_resource(
       (int *)&resource_info_state, (int *)&resource_info) == 0)
    {
      return report_failed_application_start(
        "load_front_end_cel_resource", -1, path);
    }
  else
    {
      queue_fullscreen_clear();
      draw_front_end_cel(0);
      present_display_frame(1);
      finalize_front_end_cel_resource(0, 1);
    }

  RSRC_DisposeFileResources(resource);
  ScavengeMem();

  if(runtime_feature_flags.streaming_audio_enabled.value != 0)
    {
      enable_movie_playback();
      result = initialize_movie_subscribers();
      if(result < 0)
        {
          return report_failed_application_start(
            "initialize_movie_subscribers", result, intro_stream);
        }

      play_movie_stream(intro_stream,
                        MOVIE_OPTION_CLEAR_AFTER_PLAYBACK);
      result = dispose_movie_subscribers();
      if(result < 0)
        {
          return report_failed_application_start(
            "dispose_movie_subscribers", result, intro_stream);
        }
    }
  initialize_rider_progression_profiles();

  for(;;)
    {
      set_display_vbl_synchronization_enabled(0);
      ScavengeMem();
      MEM_CompactHeap(MEM_COMPACT_TARGET_DRAM |
                      MEM_COMPACT_TARGET_VRAM |
                      MEM_COMPACT_PURGE_HANDLES);
      configure_display_clipping(FRONT_END_CLIP_WIDTH,
                                 FRONT_END_CLIP_HEIGHT);
      result = run_front_end_menu_loop();
      if(result < 0)
        {
          return report_failed_application_start(
            "run_front_end_menu_loop", result, 0);
        }
      ScavengeMem();
      MEM_CompactHeap(MEM_COMPACT_TARGET_DRAM |
                      MEM_COMPACT_TARGET_VRAM |
                      MEM_COMPACT_PURGE_HANDLES);
      update_music_option_for_memory_pressure();

      if(runtime_feature_flags.music_option_enabled.value == 0)
        {
          race_mode_state.music_enabled = 0;
        }
      if(runtime_feature_flags.alternate_hud_layout.value != 0)
        {
          select_text_font(resource_catalog_handles.large_digit_font);
        }
      else
        {
          select_text_font(resource_catalog_handles.small_digit_font);
        }

      set_text_color(TEXT_COLOR_MAX_COMPONENT,
                     TEXT_COLOR_MAX_COMPONENT,
                     TEXT_COLOR_MAX_COMPONENT);
      load_rash_options_resources();
      session = load_course_resources();
      if(session == 0 || session->payload.record == 0 ||
         session->payload.record->family_schedule_resource == 0)
        {
          if(discard_failed_race_start(course_resource_stage, 0, 0, 0) < 0)
            {
              return 1;
            }
          continue;
        }
      set_display_vbl_synchronization_enabled(1);
      family_initialized = initialize_family_resource_runtime(
        session->payload.record->family_schedule_resource);
      if(!family_initialized)
        {
          if(discard_failed_race_start(family_resource_stage, 0, 0, 0) < 0)
            {
              return 1;
            }
          continue;
        }

      /* These sidecars are reconstruction-only lifetime guards.  Reset them
         before the oracle-owned srand/GetAudioTime interval so they do not
         add work between seeding and the rider discard sequence. */
      reset_all_spatial_audio_event_family_bindings();
      reset_all_rider_visual_resource_bindings();
      reset_all_roadside_collision_binding_states();
      reset_all_racer_auxiliary_cache_bindings();
      for(cache_index = 0;
          cache_index < ROAD_TEXTURE_CACHE_COUNT;
          cache_index++)
        {
          reset_road_texture_binding_state(
            &gRoadTextureBindingStates[cache_index]);
        }

      race_audio_initialized = 0;
      if(runtime_feature_flags.game_audio_enabled.value != 0)
        {
          result = load_race_audio_collection();
          runtime_feature_flags.game_audio_enabled.value =
            (unsigned char)(result == 0);
          if(runtime_feature_flags.game_audio_enabled.value != 0)
            {
              result = load_score();
              if(result < 0)
                {
                  printf(race_score_failure_message);
                  if(unload_score() < 0)
                    {
                      return 1;
                    }
                  game_audio_state.score_enabled = 0;
                }
              result = initialize_race_audio();
              runtime_feature_flags.game_audio_enabled.value =
                (unsigned char)(result == 0);
            }
          race_audio_initialized =
            runtime_feature_flags.game_audio_enabled.value != 0;
          if(!race_audio_initialized)
            {
              printf(race_audio_failure_message);
              if(unload_score() < 0 || shutdown_race_audio() < 0)
                {
                  return 1;
                }
            }
        }

      frame_timer_enabled.value = 0;
      frame_tick = 0;
      renderer_initialized = initialize_road_renderer_worker();
      if(!renderer_initialized)
        {
          if(discard_failed_race_start(
               renderer_worker_stage,
               family_initialized, 0, race_audio_initialized) < 0)
            {
              return 1;
            }
          continue;
        }
      if(!initialize_track_simulation(session))
        {
          if(discard_failed_race_start(
               track_simulation_stage,
               family_initialized, renderer_initialized,
               race_audio_initialized) < 0)
            {
              return 1;
            }
          continue;
        }

      if(race_audio_initialized)
        {
          if(start_score() < 0)
            {
              printf(race_score_failure_message);
              if(unload_score() < 0)
                {
                  return 1;
                }
              game_audio_state.score_enabled = 0;
            }
          if(start_race_engine_audio() < 0)
            {
              printf(race_audio_failure_message);
              if(unload_score() < 0 || shutdown_race_audio() < 0)
                {
                  return 1;
                }
              runtime_feature_flags.game_audio_enabled.value = 0;
              race_audio_initialized = 0;
            }
        }
      ScavengeMem();

      queue_fullscreen_clear();
      present_display_frame(1);
      queue_fullscreen_clear();
      present_display_frame(1);
      configure_display_clipping(RACE_CLIP_WIDTH, RACE_CLIP_HEIGHT);
      race_result = run_race_input_and_simulation_loop();
      set_display_vbl_synchronization_enabled(0);
      stop_road_renderer_worker();
      shutdown_track_simulation();
      if(shutdown_family_resource_runtime() < 0)
        {
          return 1;
        }

      if(race_audio_initialized)
        {
          if(unload_score() < 0 || shutdown_race_audio() < 0)
            {
              return 1;
            }
        }

      unload_course_resources();
      unload_rash_options_resources();
      if(race_result < 0)
        {
          return report_failed_application_start(
            "run_race_input_and_simulation_loop", race_result, 0);
        }
    }
}
