#include "stddef.h"

#include "audio.h"

#include "async_stream_runtime.h"

typedef char AsyncStreamControlSubtypeOffsetIs0x10[
    offsetof(AsyncStreamControlChunk, subchunk_type) == 0x10 ? 1 : -1];
typedef char AsyncStreamControlValueOffsetIs0x14[
    offsetof(AsyncStreamControlChunk, control_value) == 0x14 ? 1 : -1];
typedef char AsyncStreamAudioClockOriginOffsetIs0xB4[
    offsetof(AsyncStreamPlaybackStatePrefix, audio_clock_origin) == 0xB4
        ? 1 : -1];
typedef char AsyncStreamScheduledFrameTimeOffsetIs0xB8[
    offsetof(AsyncStreamPlaybackStatePrefix, scheduled_frame_time) == 0xB8
        ? 1 : -1];

/* Reconstructed from the original function at 0x00031840. */
void
apply_stream_control_clock_sync(const AsyncStreamControlChunk  *chunk,
                                AsyncStreamPlaybackStatePrefix *state)
{
  if(chunk->subchunk_type == ASYNC_STREAM_CONTROL_SUBTYPE_CUES)
    {
      return;
    }
  if(chunk->subchunk_type != ASYNC_STREAM_CONTROL_SUBTYPE_SYNC)
    {
      return;
    }

  state->audio_clock_origin =
    (unsigned int)GetAudioTime() - chunk->control_value;
}
