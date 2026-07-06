#include "application_runtime.h"
#include "cpak_player_runtime.h"
#include "data_stream_runtime.h"
#include "stream_input_runtime.h"

#define MOVIE_EXIT_BUTTON_MASK 0x0E000000U
#define MOVIE_VOLUME_DOWN_BUTTON 0x00200000U
#define MOVIE_VOLUME_UP_BUTTON 0x00400000U
#define MOVIE_VOLUME_MINIMUM 0x600
#define MOVIE_VOLUME_MAXIMUM 0x7A00
#define MOVIE_VOLUME_STEP 0x500
#define ROADRASH_SNDS_CHUNK_TYPE CHAR4LITERAL('S', 'N', 'D', 'S')

int32
service_movie_stream_playback(RoadRashCPakPlayer           *player,
                              RoadRashMoviePlaybackRequest *request)
{
  SAudioControlBlock audio_control;
  RoadRashControlCueRequest *cue_requests;
  uint32 stream_time;
  uint32 buttons;
  int32 request_index;
  int32 status;
  const char *stream_path;

  stream_path = player->acquisition_context != 0 ?
    player->acquisition_context->file_name : 0;
  if((player->stream_context->streamFlags & ROADRASH_STREAM_ABORTED_FLAG) != 0)
    {
      show_application_error("Movie: stream aborted", ROADRASH_DS_ABORT_ERROR,
                             stream_path);
    }

  if(request->cue_request_count > 0)
    {
      cue_requests = &request->frame_transition_cue;
      dispatch_due_control_cues(
        player->control_context, request->cue_request_count,
        cue_requests);
      for(request_index = 0;
          request_index < request->cue_request_count;
          request_index++)
        {
          if(cue_requests[request_index].callback != 0)
            {
              break;
            }
        }
      if(request_index >= request->cue_request_count)
        {
          request->cue_request_count = 0;
        }
    }

  status = DSGetClock(player->stream_context, &stream_time);
  if(status == ROADRASH_DS_CLOCK_NOT_VALID_ERROR)
    {
      return 1;
    }
  if(status < 0)
    {
      show_application_error("Movie: DSGetClock", status, stream_path);
    }

  sample_stream_input_pads();
  buttons = get_stream_input_buttons(1);
  if((buttons & MOVIE_EXIT_BUTTON_MASK) != 0)
    {
      return 1;
    }

  if(request->volume_controls_enabled == 0)
    {
      return 0;
    }

  if((buttons & MOVIE_VOLUME_DOWN_BUTTON) != 0)
    {
      audio_control.amplitude.channel = request->audio_channel;
      status = control_legacy_data_stream(
        player->request_message, 0, player->stream_context,
        ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_GET_AMPLITUDE,
        &audio_control);
      if(status < 0)
        {
          show_application_error("Movie: get audio amplitude", status,
                                 stream_path);
        }

      if(audio_control.amplitude.value > MOVIE_VOLUME_MINIMUM)
        {
          audio_control.amplitude.value -= MOVIE_VOLUME_STEP;
          audio_control.amplitude.channel = request->audio_channel;
          status = control_legacy_data_stream(
            player->request_message, 0, player->stream_context,
            ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_SET_AMPLITUDE,
            &audio_control);
          if(status < 0)
            {
              show_application_error("Movie: set audio amplitude", status,
                                     stream_path);
            }
        }
    }

  if(request->volume_controls_enabled != 0 &&
     (buttons & MOVIE_VOLUME_UP_BUTTON) != 0)
    {
      audio_control.amplitude.channel = request->audio_channel;
      status = control_legacy_data_stream(
        player->request_message, 0, player->stream_context,
        ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_GET_AMPLITUDE,
        &audio_control);
      if(status < 0)
        {
          show_application_error("Movie: get audio amplitude", status,
                                 stream_path);
        }

      if(audio_control.amplitude.value < MOVIE_VOLUME_MAXIMUM)
        {
          audio_control.amplitude.value += MOVIE_VOLUME_STEP;
          audio_control.amplitude.channel = request->audio_channel;
          status = control_legacy_data_stream(
            player->request_message, 0, player->stream_context,
            ROADRASH_SNDS_CHUNK_TYPE, SAUDIO_CONTROL_SET_AMPLITUDE,
            &audio_control);
          if(status < 0)
            {
              show_application_error("Movie: set audio amplitude", status,
                                     stream_path);
            }
        }
    }

  return 0;
}
