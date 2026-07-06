#include "audio.h"
#include "codec.h"
#include "graphics.h"
#include "item.h"

#include "async_stream_playback_helpers.h"
#include "async_stream_runtime.h"
#include "rw_semantic_data.h"
#include "stream_audio_playback.h"

enum
{
  ASYNC_STREAM_DEFAULT_AUDIO_CHANNEL_COUNT = 2,
  ASYNC_STREAM_CHUNK_ALIGNMENT = 4,
  ASYNC_STREAM_CHUNK_ALIGNMENT_MASK = ASYNC_STREAM_CHUNK_ALIGNMENT - 1
};

int
get_stream_audio_playback_amplitude(void);

static
int
find_stream_tag_value(unsigned int         tag,
                      const RwTagArgument *tags)
{
  return (int)find_stream_tag_argument(
    tag, tags, stream_open_tags)->argument;
}


static
int
did_stream_time_cross(unsigned int previous,
                      unsigned int current,
                      unsigned int value)
{
  if(previous < current)
    {
      return value > previous && value < current;
    }
  return value < current || value > previous;
}


static
void
resynchronize_stream_audio(AsyncStreamAudioSubmitCallback   callback,
                           AsyncStreamAudioPlaybackContext *audio)
{
  if(callback != 0 &&
     audio->scheduled_end_time != audio->state->audio_clock_origin)
    {
      callback(0, 0);
      callback(0, 0);
      audio->scheduled_end_time = audio->state->audio_clock_origin;
    }
}


static
void
process_async_stream_chunk_buffer(AsyncStreamPlaybackStatePrefix  *state,
                                  AsyncBlockStream                *block_stream,
                                  AsyncStreamChunkHeader          *first_chunk,
                                  unsigned char                   *paused,
                                  unsigned char                   *continue_playback,
                                  int                              audio_callback_active,
                                  AsyncStreamAudioPlaybackContext *audio,
                                  AsyncStreamFilmPlaybackContext  *film,
                                  int                             *status,
                                  int                             *wait_for_message)
{
  AsyncStreamChunkHeader *chunk;
  unsigned char *cursor;
  unsigned char *end;
  unsigned char *fill_limit;
  int remaining_bytes;
  int chunk_size;

  *wait_for_message = 0;
  cursor = (unsigned char *)first_chunk;
  if(state->io_block_bytes < (int)sizeof(AsyncStreamChunkHeader))
    {
      *status = ASYNC_STREAM_ERROR_INVALID_CHUNK;
      *continue_playback = 0;
      release_async_block_stream_chunk(block_stream, first_chunk);
      return;
    }
  end = cursor + state->io_block_bytes;
  while(cursor < end)
    {
      remaining_bytes = (int)(end - cursor);
      if(remaining_bytes < (int)sizeof(AsyncStreamChunkHeader))
        {
          *status = ASYNC_STREAM_ERROR_INVALID_CHUNK;
          *continue_playback = 0;
          break;
        }

      chunk = (AsyncStreamChunkHeader *)cursor;
      chunk_size = chunk->chunk_size;
      if(chunk_size < (int)sizeof(AsyncStreamChunkHeader) ||
         (chunk_size & ASYNC_STREAM_CHUNK_ALIGNMENT_MASK) != 0 ||
         chunk_size > remaining_bytes)
        {
          *status = ASYNC_STREAM_ERROR_INVALID_CHUNK;
          *continue_playback = 0;
          break;
        }

      *status = service_async_stream_messages(
        state, paused, continue_playback, 0, audio_callback_active);
      if(*status == ASYNC_STREAM_DISPATCH_STOP || *status < 0)
        {
          break;
        }

      if(chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_FILM ||
         chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_SOUND ||
         chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_STREAMED_ANIMATION)
        {
          present_pending_async_stream_frame(chunk, state);
          if(block_stream->last_error < 0)
            {
              *status = block_stream->last_error;
              *continue_playback = 0;
              break;
            }
        }

      if(chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_FILM)
        {
          process_async_stream_film_chunk(chunk, film);
        }
      else if(chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_CONTROL)
        {
          apply_stream_control_clock_sync(
            (const AsyncStreamControlChunk *)chunk, state);
        }
      else if(chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_FILL)
        {
          fill_limit = (unsigned char *)first_chunk +
                       state->io_block_bytes -
                       sizeof(AsyncStreamChunkHeader);
          if(cursor >= fill_limit)
            {
              break;
            }
        }
      else if(chunk->chunk_type == ASYNC_STREAM_CHUNK_TYPE_SOUND)
        {
          process_async_stream_audio_chunk(chunk, audio);
          state->applied_audio_amplitude =
            get_stream_audio_playback_amplitude();
        }

      if(block_stream->last_error < 0)
        {
          *status = block_stream->last_error;
          *continue_playback = 0;
          break;
        }

      cursor += chunk_size;
    }

  release_async_block_stream_chunk(block_stream, first_chunk);
}


/* Reconstructed from the original function at 0x00031894. */
int
run_async_stream_playback(AsyncStreamPlaybackStatePrefix *state)
{
  const RwTagArgument *tags;
  AsyncBlockStream *block_stream;
  codecHandler codec_handler;
  codec film_codec;
  int video_enabled;
  int status;
  int wait_for_message;
  int wait_for_buffer;
  AsyncStreamChunkHeader *chunk;
  unsigned int now;
  unsigned char paused;
  unsigned char continue_playback;
  AsyncStreamAudioSubmitCallback callback;
  AsyncStreamAudioPlaybackContext audio;
  AsyncStreamFilmPlaybackContext film;

  tags = state->tag_overrides;
  wait_for_buffer = (unsigned char)find_stream_tag_value(
    ASYNC_STREAM_TAG_WAIT_FOR_BUFFER, tags);
  audio.initialize_callback =
    (AsyncStreamAudioInitializeCallback)find_stream_tag_value(
      ASYNC_STREAM_TAG_AUDIO_INITIALIZE_CALLBACK, tags);
  audio.submit_callback =
    (AsyncStreamAudioSubmitCallback)find_stream_tag_value(
      ASYNC_STREAM_TAG_AUDIO_CHUNK_CALLBACK, tags);
  callback = audio.submit_callback;
  if(callback != 0)
    {
      state->requested_audio_amplitude = find_stream_tag_value(
        ASYNC_STREAM_TAG_AUDIO_AMPLITUDE, tags);
    }

  audio.state = state;
  audio.channel_count = ASYNC_STREAM_DEFAULT_AUDIO_CHANNEL_COUNT;
  if(state->first_screen_item > 0)
    {
      status = DisplayScreen(state->first_screen_item, 0);
      if(status < 0)
        {
          return status;
        }
    }
  state->video_copy_io_request_item = GetVRAMIOReq();
  if(state->video_copy_io_request_item < 0)
    {
      status = state->video_copy_io_request_item;
      state->video_copy_io_request_item = 0;
      return status;
    }
  state->audio_cue_item = CreateSizedItem(
    MKNODEID(AUDIONODE, AUDIO_CUE_NODE), 0, 0);
  if(state->audio_cue_item < 0)
    {
      status = state->audio_cue_item;
      state->audio_cue_item = 0;
      DeleteItem(state->video_copy_io_request_item);
      state->video_copy_io_request_item = 0;
      return status;
    }

  film.state = state;
  film.bitmap = (Bitmap *)find_stream_tag_value(
    ASYNC_STREAM_TAG_FRAME_BITMAP, tags);
  film.horizontal_position = find_stream_tag_value(
    ASYNC_STREAM_TAG_FRAME_X, tags);
  film.vertical_position = find_stream_tag_value(
    ASYNC_STREAM_TAG_FRAME_Y, tags);
  if(film.bitmap == (Bitmap *)-1)
    {
      film.bitmap = state->second_screen_bitmap;
    }
  film.cel_list = (CCB *)find_stream_tag_value(
    ASYNC_STREAM_TAG_FRAME_CEL_LIST, tags);
  if(film.cel_list == (CCB *)&stream_frame_cel_terminator_flags)
    {
      stream_frame_cel_terminator_flags |= CCB_SKIP | CCB_LAST;
    }

  codec_handler = 0;
  film_codec = 0;
  video_enabled = film.bitmap != 0;
  if(video_enabled != 0)
    {
      if(state->first_screen_bitmap == 0 ||
         state->second_screen_bitmap == 0)
        {
          status = ASYNC_BLOCK_STREAM_ERROR_BAD_POINTER;
          goto cleanup;
        }
      codec_handler = CreateCodecHandler();
      if(codec_handler != 0)
        {
          film_codec = CreateCodec(codec_handler, 0);
        }
      if(film_codec == 0)
        {
          status = -1;
          goto cleanup;
        }
    }
  film.film_codec = film_codec;

  for(;;)
    {
      if(wait_for_buffer != 0 &&
         state->block_stream->buffering_enabled != 0)
        {
          status = set_async_block_stream_buffering(
            state->block_stream, 1, 1);
          if(status < 0)
            {
              goto cleanup;
            }
        }

      state->audio_clock_origin = (unsigned int)GetAudioTime();
      state->frame_copy_pending = 0;
      audio.awaiting_first_sample = 1;
      audio.scheduled_end_time = state->audio_clock_origin;
      if(callback != 0)
        {
          status = retain_stream_audio_playback();
          if(status < 0)
            {
              goto cleanup;
            }
        }

      continue_playback = 1;
      wait_for_message = 0;
      status = 0;
      for(;;)
        {
          now = (unsigned int)GetAudioTime();
          if(callback != 0 &&
             audio.scheduled_end_time != state->audio_clock_origin &&
             did_stream_time_cross(
               audio.scheduled_end_time,
               state->audio_clock_origin,
               now))
            {
              callback(0, 0);
              callback(0, 0);
              audio.scheduled_end_time = state->audio_clock_origin;
            }

          paused = 0;
          status = service_async_stream_messages(
            state, &paused, &continue_playback,
            wait_for_message, callback != 0);
          if(status == ASYNC_STREAM_DISPATCH_STOP || status < 0)
            {
              break;
            }

          block_stream = state->block_stream;
          if(callback != 0 &&
             audio.scheduled_end_time != state->audio_clock_origin &&
             poll_async_block_stream_readiness(block_stream) == 0)
            {
              if(continue_playback == 0)
                {
                  break;
                }
              continue;
            }

          chunk = (AsyncStreamChunkHeader *)
                  acquire_async_block_stream_chunk(block_stream);
          if(chunk == 0)
            {
              status = get_async_block_stream_status(block_stream);
              if(status == ASYNC_BLOCK_STREAM_STATUS_END_OF_STREAM)
                {
                  continue_playback = 0;
                  break;
                }
              if(status != ASYNC_BLOCK_STREAM_STATUS_BUFFERING)
                {
                  break;
                }
              if(block_stream->buffering_enabled != 0)
                {
                  status = ASYNC_STREAM_DISPATCH_STOP;
                  break;
                }
              wait_for_message =
                callback == 0 ||
                audio.scheduled_end_time == state->audio_clock_origin;
            }
          else
            {
              process_async_stream_chunk_buffer(
                state, block_stream, chunk, &paused,
                &continue_playback, callback != 0, &audio, &film,
                &status, &wait_for_message);
            }

          if(continue_playback == 0)
            {
              break;
            }
        }

      if(audio.awaiting_first_sample == 0 && status != 0)
        {
          SleepUntilTime(state->audio_cue_item,
                         audio.scheduled_end_time);
        }
      if(callback != 0)
        {
          resynchronize_stream_audio(callback, &audio);
          release_stream_audio_playback();
        }

      if(state->repeat != 0 &&
         status == ASYNC_BLOCK_STREAM_STATUS_END_OF_STREAM)
        {
          status = seek_async_block_stream(state->block_stream, 0, 0);
          if(status >= 0)
            {
              continue;
            }
        }

      break;
    }

  cleanup:
  DeleteItem(state->video_copy_io_request_item);
  state->video_copy_io_request_item = 0;
  DeleteItem(state->audio_cue_item);
  state->audio_cue_item = 0;
  if(film_codec != 0)
    {
      DisposeCodec(film_codec);
    }
  if(codec_handler != 0)
    {
      DisposeCodecHandler(codec_handler);
    }
  return status;
}
