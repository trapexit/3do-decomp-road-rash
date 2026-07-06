#include "io.h"
#include "task.h"

#include "application_runtime.h"
#include "cpak_player_runtime.h"
#include "data_stream_runtime.h"
#include "display_system_runtime.h"

#define CPAK_STREAM_START_OPTIONS 2
#define CPAK_STREAM_STOP_FLUSH 1
#define CPAK_VRAM_COPY_PAGE_COUNT 75
#define CPAK_SCREEN_BUFFER_COUNT 2

int
get_active_display_screen_index(void);
void
select_display_system_screen(int screen_index);

int32
play_prepared_cpak_stream(RoadRashCPakPlayer           *player,
                          RoadRashMovieScreenContext   *screen_context,
                          DSRequestMsgPtr              end_of_stream_request,
                          int32                         horizontal_offset,
                          int32                         vertical_offset,
                          RoadRashCPakPlayerCallback    callback,
                          RoadRashMoviePlaybackRequest *callback_request)
{
  int32 status;
  int32 callback_result;
  int32 current_frame_status;
  int32 displayed_frame_status;
  int32 frame_changed;
  int32 first_frame;
  int32 saved_display_index;
  Message *reply_message;
  const char *stream_path;
  Bitmap *current_bitmap;
  Bitmap *next_bitmap;

  stream_path = player->acquisition_context != 0 ?
    player->acquisition_context->file_name : 0;

  player->callback_request = callback_request;
  player->screen_context = screen_context;
  player->callback = callback;

  status = start_legacy_data_stream(player->request_message, 0,
                                    player->stream_context,
                                    CPAK_STREAM_START_OPTIONS);
  if(status != 0)
    {
      show_application_error("Movie: start stream", status, stream_path);
      return status;
    }

  status = wait_for_legacy_data_stream_end(player->end_of_stream_message,
                                           end_of_stream_request,
                                           player->stream_context);
  if(status != 0)
    {
      show_application_error("Movie: request EOF notification", status,
                             stream_path);
      return status;
    }

  current_bitmap = player->screen_context->bitmaps[
    player->screen_context->current_screen];
  status = DisplayScreen(
    player->screen_context->screen_items[
      player->screen_context->current_screen],
    0);
  if(status < 0)
    {
      show_application_error("Movie: DisplayScreen", status, stream_path);
    }
  next_bitmap = player->screen_context->bitmaps[
    (player->screen_context->current_screen + 1) %
    CPAK_SCREEN_BUFFER_COUNT];

  displayed_frame_status = 0;
  current_frame_status = 0;
  first_frame = 1;
  callback_result = 0;

  for(;;)
    {
      if(player->callback != 0)
        {
          callback_result = player->callback(
            player, player->callback_request);
          if(callback_result < 0)
            {
              show_application_error("Movie: playback callback",
                                     callback_result, stream_path);
            }
          if(callback_result != 0)
            {
              break;
            }
        }

      if(PollForMsg(player->reply_port, 0, &reply_message, 0, &status) != 0)
        {
          status = (int32)reply_message->msg_Result;
          if(status < 0 && status != ROADRASH_DS_END_OF_FILE_ERROR)
            {
              show_application_error("Movie: stream completion",
                                     status, stream_path);
            }
          break;
        }
      if(status < 0)
        {
          show_application_error("Movie: poll stream completion", status,
                                 stream_path);
        }

      frame_changed = 0;
      if(player->cpak_context != 0)
        {
          if(displayed_frame_status == current_frame_status)
            {
              status = WaitIO(player->vram_io_request);
              if(status < 0)
                {
                  show_application_error("Movie: wait VRAM copy", status,
                                         stream_path);
                }

              if(first_frame != 0)
                {
                  saved_display_index = get_active_display_screen_index();
                  select_display_system_screen(
                    (player->screen_context->current_screen + 1) %
                    CPAK_SCREEN_BUFFER_COUNT);
                  fill_active_display_bitmap(0);
                  select_display_system_screen(saved_display_index);
                  first_frame = 0;
                }

              status = draw_cpak_frame_to_buffer(player->cpak_context,
                                                 player->cpak_record,
                                                 next_bitmap,
                                                 horizontal_offset,
                                                 vertical_offset);
              if(status != current_frame_status)
                {
                  current_frame_status = status;
                  frame_changed = 1;
                }
            }

          if(displayed_frame_status != current_frame_status &&
             is_cpak_frame_ready_for_display(player->cpak_record) != 0)
            {
              status = CopyVRAMPagesDefer(player->vram_io_request,
                                 current_bitmap->bm_Buffer,
                                 next_bitmap->bm_Buffer,
                                 CPAK_VRAM_COPY_PAGE_COUNT, (uint32) - 1);
              if(status < 0)
                {
                  show_application_error("Movie: copy frame to VRAM", status,
                                         stream_path);
                }
              displayed_frame_status = current_frame_status;
              frame_changed = 1;
            }

          signal_cpak_freed_chunks(player->cpak_context);
        }

      if(frame_changed == 0)
        {
          Yield();
        }
    }

  status = stop_legacy_data_stream(player->request_message, 0,
                                   player->stream_context,
                                   CPAK_STREAM_STOP_FLUSH);
  if(status < 0)
    {
      show_application_error("Movie: stop stream", status, stream_path);
    }
  if(player->cpak_context != 0)
    {
      status = flush_cpak_cel(player->cpak_context, player->cpak_record, 0);
      if(status < 0)
        {
          show_application_error("Movie: flush Cinepak cel", status,
                                 stream_path);
        }
    }

  return callback_result;
}
