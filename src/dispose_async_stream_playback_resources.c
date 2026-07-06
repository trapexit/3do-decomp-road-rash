#include "stddef.h"

#include "item.h"

#include "async_stream_runtime.h"

typedef char AsyncStreamFirstScreenItemOffsetIs0xC8[
    offsetof(AsyncStreamPlaybackStatePrefix, first_screen_item) == 0xC8
        ? 1 : -1];
typedef char AsyncStreamSecondScreenItemOffsetIs0xCC[
    offsetof(AsyncStreamPlaybackStatePrefix, second_screen_item) == 0xCC
        ? 1 : -1];
typedef char AsyncStreamPlaybackPrefixSizeIs0xE8[
    sizeof(AsyncStreamPlaybackStatePrefix) == 0xE8 ? 1 : -1];

/* Reconstructed from the original function at 0x00031638. */
int
dispose_async_stream_playback_resources(AsyncStreamPlaybackStatePrefix *state)
{
  if(state->first_screen_item != 0)
    {
      CloseItem(state->first_screen_item);
    }
  if(state->second_screen_item != 0)
    {
      CloseItem(state->second_screen_item);
    }

  return dispose_async_block_stream(state->block_stream);
}
