#include "stdio.h"

#include "audio.h"
#include "event.h"
/* Reconstructed from the original function at 0x00033534. */

#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

static const char s_set_buffering_false[] = "Set buffering false returned %ld\n";

#define BACKGROUND_AUDIO_GAIN_SOFT_MAX 0x7F00
#define BACKGROUND_AUDIO_GAIN_HARD_MAX 0x7FFF
#define BACKGROUND_AUDIO_BUFFERING_DIRECT_LIMIT 0x168
#define BACKGROUND_AUDIO_BOOLEAN_MASK 0xFF

void
adjust_background_audio_gain_from_input(int flags)
{
  BackgroundAudioGlobals *state;
  int now;
  int next;

  state = &background_audio_globals;
  if(state->stream_state == 0)
    {
      return;
    }

  now = GetAudioTime();
  if(now <= state->last_adjustment_time + state->gain_cap)
    {
      return;
    }

  if((flags & ControlLeftShift) != 0 &&
     state->current_gain >= state->gain_step)
    {
      adjust_async_stream_gain(
        (AsyncStreamPlaybackStatePrefix *)state->stream_state,
        -state->gain_step);
      state->current_gain -= state->gain_step;
      state->last_adjustment_time = GetAudioTime();
    }

  if((flags & ControlRightShift) == 0)
    {
      return;
    }

  next = state->current_gain + state->gain_step;
  if(next < BACKGROUND_AUDIO_GAIN_SOFT_MAX ||
     next <= BACKGROUND_AUDIO_GAIN_HARD_MAX)
    {
      adjust_async_stream_gain(
        (AsyncStreamPlaybackStatePrefix *)state->stream_state,
        state->gain_step);
      state->current_gain += state->gain_step;
      state->last_adjustment_time = GetAudioTime();
    }
}


void
set_background_audio_buffering(int enabled,
                               int threshold)
{
  BackgroundAudioGlobals *state;
  int result;
  int count;

  state = &background_audio_globals;
  state->enabled = (unsigned char)enabled;
  if(state->stream_state == 0)
    {
      return;
    }

  if(threshold < BACKGROUND_AUDIO_BUFFERING_DIRECT_LIMIT)
    {
      set_async_stream_buffering_mode(state->stream_state,
                                      enabled & BACKGROUND_AUDIO_BOOLEAN_MASK,
                                      0, ASYNC_STREAM_UPDATE_STATE_ONLY);
      return;
    }

  if((enabled & BACKGROUND_AUDIO_BOOLEAN_MASK) == 0)
    {
      result = set_async_stream_buffering_mode(state->stream_state, 0, 0, &count);
      if(result != 0)
        {
          printf(s_set_buffering_false, result);
          return;
        }

      if(count < threshold)
        {
          set_async_stream_buffering_mode(state->stream_state, 1, 0, &count);
          result = set_async_stream_buffering_mode(state->stream_state, 0, 1, &count);
          if(result != 0)
            {
              printf(s_set_buffering_false, result);
            }
        }
    }
  else
    {
      result = set_async_stream_buffering_mode(state->stream_state, 1, 0, &count);
      if(result != 0)
        {
          printf(s_set_buffering_false, result);
        }
    }
}


#undef BACKGROUND_AUDIO_GAIN_SOFT_MAX
#undef BACKGROUND_AUDIO_GAIN_HARD_MAX
#undef BACKGROUND_AUDIO_BUFFERING_DIRECT_LIMIT
#undef BACKGROUND_AUDIO_BOOLEAN_MASK
