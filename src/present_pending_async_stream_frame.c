#include "audio.h"
#include "graphics.h"

#include "async_stream_playback_helpers.h"

/* Reconstructed from the original function at 0x000322E8. */
void
present_pending_async_stream_frame(const AsyncStreamChunkHeader   *next_chunk,
                                   AsyncStreamPlaybackStatePrefix *state)
{
  unsigned int next_chunk_time;
  int result;

  if(state->frame_copy_pending == 0)
    {
      return;
    }

  if(next_chunk != 0)
    {
      next_chunk_time =
        state->audio_clock_origin + next_chunk->stream_time;
      if(next_chunk_time <= state->scheduled_frame_time)
        {
          return;
        }
    }

  result = SleepUntilTime(state->audio_cue_item, state->scheduled_frame_time);
  if(result < 0)
    {
      state->block_stream->last_error = result;
      return;
    }
  result = CopyVRAMPagesDefer(state->video_copy_io_request_item,
                             state->first_screen_bitmap->bm_Buffer,
                             state->second_screen_bitmap->bm_Buffer,
                             ASYNC_STREAM_FRAME_COPY_PAGE_COUNT,
                             -1);
  if(result < 0)
    {
      state->block_stream->last_error = result;
      return;
    }
  state->frame_copy_pending = 0;
}
