#include "async_stream_runtime.h"

void
adjust_async_stream_gain(AsyncStreamPlaybackStatePrefix *state,
                         int                             amplitude_delta)
{
  int next_amplitude;

  if(state == 0)
    {
      return;
    }

  next_amplitude = state->requested_audio_amplitude + amplitude_delta;
  if(next_amplitude < 0)
    {
      return;
    }

  if(next_amplitude <= ASYNC_STREAM_AUDIO_AMPLITUDE_MAX)
    {
      state->requested_audio_amplitude = next_amplitude;
    }
}
