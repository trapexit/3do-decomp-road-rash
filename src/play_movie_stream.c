#include "stdio.h"
#include "string.h"

#include "graphics.h"
#include "io.h"

#include "application_runtime.h"
#include "cpak_player_runtime.h"
#include "data_stream_runtime.h"
#include "display_system_runtime.h"
#include "movie_playback_runtime.h"
#include "path_runtime.h"
#include "rw_semantic_data.h"
#include "stream_input_runtime.h"

void
queue_fullscreen_clear(int screen_index);
void
present_display_frame(int screen_index);

/* Retain ownership across a failed teardown instead of losing stack state. */
static RoadRashCPakPlayer player;
static DSRequestMsg end_of_stream_request;
static char stream_path[MOVIE_STREAM_PATH_BYTES];

int
release_movie_playback_resources(void)
{
  int status;

  status = dismantle_cpak_player(&player);
  if(status < 0)
    {
      return status;
    }
  return stop_retained_movie_stream();
}

static
void
flip_display_pair(void)
{
  queue_fullscreen_clear(0);
  present_display_frame(1);
  queue_fullscreen_clear(0);
  present_display_frame(1);
}


static
int32
next_movie_buffer_index(int32 buffer_index)
{
  buffer_index += 1;
  if(buffer_index < 0)
    {
      return -(buffer_index & 1);
    }
  return buffer_index & 1;
}


void
play_movie_stream(const char *stream_name,
                  int32       options)
{
  RoadRashMoviePlaybackRequest request;
  int32 status;
  uint32 buttons;
  int32 decode_limit;
  int32 buffered_count;
  int32 half_buffer_count;

  status = release_movie_playback_resources();
  if(status < 0)
    {
      show_application_error("Movie: release previous stream", status,
                             stream_name);
      return;
    }

  if((options & MOVIE_OPTION_ASYNCHRONOUS) != 0)
    {
      play_async_movie_stream(stream_name, options);
      return;
    }

  if(copy_path_with_capacity(
       stream_path, sizeof(stream_path), resource_root_path) < 0 ||
     append_path_with_capacity(
       stream_path, sizeof(stream_path), MOVIE_STREAM_DIRECTORY) < 0 ||
     append_path_with_capacity(
       stream_path, sizeof(stream_path), stream_name) < 0)
    {
      show_application_error("Movie: construct stream path", -1, stream_name);
      return;
    }

  do
    {
      sample_stream_input_pads();
    } while(get_stream_input_buttons(1) != 0);

  status = prepare_cpak_player(&player, stream_path, 1);
  if(status < 0)
    {
      show_application_error("Movie: prepare", status, stream_path);
      return;
    }

  status = preroll_legacy_data_stream(
    player.request_message, 0, player.stream_context);
  if(status < 0)
    {
      show_application_error("Movie: preroll", status, stream_path);
      return;
    }

  if((options & MOVIE_OPTION_DECODE_LIMIT) == 0)
    {
      decode_limit = -1;
    }
  else
    {
      decode_limit = MOVIE_DECODE_LIMIT;
    }

  request.screen_context.current_screen = next_movie_buffer_index(
    display_runtime_globals.active_bitmap_index);
  request.screen_context.screen_items[0] =
    display_runtime_globals.screen_items[0];
  request.screen_context.screen_items[1] =
    display_runtime_globals.screen_items[1];
  request.screen_context.bitmaps[0] = display_runtime_globals.bitmaps[0];
  request.screen_context.bitmaps[1] = display_runtime_globals.bitmaps[1];

  request.audio_channel = 0;
  request.volume_controls_enabled = 1;
  request.cue_request_count = 0;

  if((options & MOVIE_OPTION_FADE_NOW) != 0)
    {
      handle_movie_frame_transition_cue(0, MOVIE_FADE_TIME);
    }
  else if((options & MOVIE_OPTION_FADE_CUE) != 0)
    {
      request.frame_transition_cue.chunk_type = FILM_CHUNK_TYPE;
      request.frame_transition_cue.subchunk_type = FRME_CHUNK_TYPE;
      request.frame_transition_cue.use_alternate_base_time = 0;
      request.frame_transition_cue.trigger_time_offset = MOVIE_FADE_TIME;
      request.frame_transition_cue.callback =
        handle_movie_frame_transition_cue;
      request.frame_transition_cue.callback_argument = &request;
      request.cue_request_count = 1;
    }

  buffered_count =
    get_prerolled_data_stream_buffer_count(player.stream_context);
  while(buffered_count >= 0)
    {
      half_buffer_count = player.header.streamBuffers;
      half_buffer_count =
        (half_buffer_count +
         (int32)((uint32)half_buffer_count >>
                 MOVIE_SIGN_BIT_SHIFT)) >> 1;
      if(half_buffer_count <= buffered_count)
        {
          break;
        }

      sample_stream_input_pads();
      buttons = get_stream_input_buttons(1);
      if((buttons & MOVIE_EXIT_BUTTON_MASK) != 0)
        {
          goto cleanup;
        }

      buffered_count =
        get_prerolled_data_stream_buffer_count(player.stream_context);
      if(buffered_count < 0)
        {
          break;
        }

      status = WaitVBLDefer(player.vbl_io_request,
                            MOVIE_PREROLL_WAIT_FIELDS);
      if(status < 0)
        {
          show_application_error("Movie: preroll WaitVBLDefer", status,
                                 stream_path);
        }
      status = WaitIO(player.vbl_io_request);
      if(status < 0)
        {
          show_application_error("Movie: preroll WaitIO", status, stream_path);
        }
    }
  if(buffered_count < 0 &&
     buffered_count != ROADRASH_DS_END_OF_FILE_ERROR)
    {
      show_application_error("Movie: preroll buffer count", buffered_count,
                             stream_path);
    }

  if((options & MOVIE_OPTION_PRESERVE_DISPLAY) == 0)
    {
      flip_display_pair();
    }
  else
    {
      copy_display_bitmap(display_runtime_globals.active_bitmap_index,
                          display_runtime_globals.active_bitmap_index ^ 1);
    }

  status = play_prepared_cpak_stream(
    &player, &request.screen_context, &end_of_stream_request, -1, decode_limit,
    service_movie_stream_playback, &request);
  if(status < 0)
    {
      show_application_error("Movie: playback callback", status, stream_path);
    }

cleanup:
  status = dismantle_cpak_player(&player);
  if(status < 0)
    {
      show_application_error("Movie: dismantle", status, stream_path);
    }

  if((options & MOVIE_OPTION_CLEAR_AFTER_PLAYBACK) != 0)
    {
      flip_display_pair();
    }
}
