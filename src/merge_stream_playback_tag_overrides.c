#include "stddef.h"

#include "async_stream_runtime.h"

typedef char AsyncStreamTagOffsetIs0x18[
    offsetof(AsyncStreamPlaybackStatePrefix, tag_overrides) == 0x18 ? 1 : -1];
typedef char AsyncStreamCommandPortOffsetIs0xA0[
    offsetof(AsyncStreamPlaybackStatePrefix, command_port_item) == 0xA0
        ? 1 : -1];

/* Reconstructed from the original function at 0x00031664. */
void
merge_stream_playback_tag_overrides(AsyncStreamPlaybackStatePrefix *state,
                                    const RwTagArgument            *incoming)
{
  const RwTagArgument *entry;

  if(incoming == 0 || incoming->tag == 0)
    {
      return;
    }

  entry = incoming;
  do
    {
      switch(entry->tag)
        {
        case ASYNC_STREAM_TAG_FRAME_X:
        case ASYNC_STREAM_TAG_FRAME_Y:
        case ASYNC_STREAM_TAG_RETAINED_EXTENSION_ONE:
        case ASYNC_STREAM_TAG_FIRST_SCREEN_ITEM:
        case ASYNC_STREAM_TAG_SECOND_SCREEN_ITEM:
        case ASYNC_STREAM_TAG_FRAME_BITMAP:
        case ASYNC_STREAM_TAG_FRAME_CEL_LIST:
        case ASYNC_STREAM_TAG_RETAINED_EXTENSION_TWO:
        case ASYNC_STREAM_TAG_AUDIO_INITIALIZE_CALLBACK:
        case ASYNC_STREAM_TAG_AUDIO_CHUNK_CALLBACK:
        case ASYNC_STREAM_TAG_AUDIO_AMPLITUDE:
        case ASYNC_STREAM_TAG_WAIT_FOR_BUFFER:
          upsert_stream_tag_argument(
            state->tag_overrides,
            ASYNC_STREAM_PLAYBACK_TAG_CAPACITY,
            entry);
          break;
        default:
          break;
        }
      entry++;
    } while(entry->tag != 0);
}
