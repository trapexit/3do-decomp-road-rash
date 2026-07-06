#include "io.h"

#include "codec.h"
#ifndef __SUBSCRIBERUTILS_H__
  #ifdef CHAR4LITERAL
    #undef CHAR4LITERAL
  #endif
#endif
#include "cpaksubscriber.h"
#include "graphics.h"

#include "async_stream_playback_helpers.h"

enum
{
  ASYNC_STREAM_SIGN_BIT_SHIFT = 31,
  ASYNC_STREAM_DIVIDE_BY_TWO_SHIFT = 1,
  ASYNC_STREAM_LRFORM_ROW_BYTES_PER_PIXEL = 2,
  ASYNC_STREAM_LRFORM_HORIZONTAL_BYTES_PER_PIXEL = 4
};

static
int
divide_by_two_towards_zero(int value)
{
  value += (int)((unsigned int)value >> ASYNC_STREAM_SIGN_BIT_SHIFT);
  return value >> ASYNC_STREAM_DIVIDE_BY_TWO_SHIFT;
}


/* Reconstructed from the original function at 0x00032368. */
void
process_async_stream_film_chunk(AsyncStreamChunkHeader         *chunk,
                                AsyncStreamFilmPlaybackContext *film)
{
  const CinePakHeader *header;
  CinePakFrame *frame;
  Bitmap *bitmap;
  int horizontal_position;
  int vertical_position;
  int result;

  bitmap = film->bitmap;

  if(bitmap == 0)
    {
      if(chunk->subchunk_type != FRME_CHUNK_TYPE)
        {
          return;
        }
      film->state->scheduled_frame_time =
        film->state->audio_clock_origin + chunk->stream_time;
      return;
    }

  if(chunk->subchunk_type == FHDR_CHUNK_TYPE)
    {
      header = (const CinePakHeader *)chunk;

      horizontal_position = film->horizontal_position;
      if(horizontal_position == -1)
        {
          horizontal_position = divide_by_two_towards_zero(
            bitmap->bm_Width - header->width);
        }

      vertical_position = film->vertical_position;
      if(vertical_position == -1)
        {
          vertical_position = divide_by_two_towards_zero(
            bitmap->bm_Height - header->height);
        }

      film->destination =
        (unsigned char *)bitmap->bm_Buffer +
        horizontal_position *
        ASYNC_STREAM_LRFORM_HORIZONTAL_BYTES_PER_PIXEL +
        (vertical_position & ~1) * bitmap->bm_Width *
        ASYNC_STREAM_LRFORM_ROW_BYTES_PER_PIXEL +
        (vertical_position & 1) *
        ASYNC_STREAM_LRFORM_ROW_BYTES_PER_PIXEL;
      film->row_bytes =
        bitmap->bm_Width * ASYNC_STREAM_LRFORM_ROW_BYTES_PER_PIXEL;
      return;
    }

  if(chunk->subchunk_type != FRME_CHUNK_TYPE)
    {
      return;
    }

  frame = (CinePakFrame *)chunk;
  result = WaitIO(film->state->video_copy_io_request_item);
  if(result < 0)
    {
      film->state->block_stream->last_error = result;
      return;
    }
  PreDecompress(film->film_codec, frame->frameData);
  Decompress(film->film_codec, frame->frameData,
             (char *)film->destination, film->row_bytes);

  if(film->bitmap != film->state->second_screen_bitmap)
    {
      result = DrawScreenCels(film->state->second_screen_item, film->cel_list);
      if(result < 0)
        {
          film->state->block_stream->last_error = result;
          return;
        }
    }

  film->state->frame_copy_pending = 1;
  film->state->scheduled_frame_time =
    film->state->audio_clock_origin + chunk->stream_time;
}
