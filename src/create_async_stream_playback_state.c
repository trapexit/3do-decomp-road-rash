#include "stddef.h"

#include "mem.h"
#include "platform.h"

#include "async_stream_runtime.h"
#include "display_system_runtime.h"
#include "division_helpers.h"
#include "rw_semantic_data.h"
#include "string.h"

void BF_GetHooks(int *allocate, int *deallocate);

typedef char AsyncStreamOpenRequestTagsOffsetIs0x08[
    offsetof(AsyncStreamOpenRequest, tags) == 0x08 ? 1 : -1];
typedef char AsyncStreamOpenRequestStackOffsetIs0x0C[
    offsetof(AsyncStreamOpenRequest, thread_stack_block) == 0x0C ? 1 : -1];
typedef char AsyncStreamOpenRequestSizeIs0x10[
    sizeof(AsyncStreamOpenRequest) == 0x10 ? 1 : -1];
typedef char AsyncStreamScheduledFrameTimeOffsetIs0xB8[
    offsetof(AsyncStreamPlaybackStatePrefix, scheduled_frame_time) == 0xB8
        ? 1 : -1];
typedef char AsyncStreamVideoCopyItemOffsetIs0xBC[
    offsetof(AsyncStreamPlaybackStatePrefix, video_copy_io_request_item) ==
    0xBC ? 1 : -1];
typedef char AsyncStreamFirstBitmapOffsetIs0xC0[
    offsetof(AsyncStreamPlaybackStatePrefix, first_screen_bitmap) == 0xC0
        ? 1 : -1];
typedef char AsyncStreamSecondBitmapOffsetIs0xC4[
    offsetof(AsyncStreamPlaybackStatePrefix, second_screen_bitmap) == 0xC4
        ? 1 : -1];
typedef char AsyncStreamFirstScreenItemOffsetIs0xC8[
    offsetof(AsyncStreamPlaybackStatePrefix, first_screen_item) == 0xC8
        ? 1 : -1];
typedef char AsyncStreamSecondScreenItemOffsetIs0xCC[
    offsetof(AsyncStreamPlaybackStatePrefix, second_screen_item) == 0xCC
        ? 1 : -1];
typedef char AsyncStreamFrameCopyPendingOffsetIs0xD0[
    offsetof(AsyncStreamPlaybackStatePrefix, frame_copy_pending) == 0xD0
        ? 1 : -1];
typedef char AsyncStreamAudioCueItemOffsetIs0xD4[
    offsetof(AsyncStreamPlaybackStatePrefix, audio_cue_item) == 0xD4
        ? 1 : -1];
typedef char AsyncStreamRequestedAmplitudeOffsetIs0xD8[
    offsetof(AsyncStreamPlaybackStatePrefix, requested_audio_amplitude) ==
    0xD8 ? 1 : -1];
typedef char AsyncStreamAppliedAmplitudeOffsetIs0xDC[
    offsetof(AsyncStreamPlaybackStatePrefix, applied_audio_amplitude) == 0xDC
        ? 1 : -1];
typedef char AsyncStreamPlaybackThreadItemOffsetIs0xE0[
    offsetof(AsyncStreamPlaybackStatePrefix, playback_thread_item) == 0xE0
        ? 1 : -1];
typedef char AsyncStreamThreadStackOffsetIs0xE4[
    offsetof(AsyncStreamPlaybackStatePrefix, thread_stack_block) == 0xE4
        ? 1 : -1];
typedef char AsyncStreamPlaybackPrefixSizeIs0xE8[
    sizeof(AsyncStreamPlaybackStatePrefix) == 0xE8 ? 1 : -1];

static AsyncStreamPlaybackStatePrefix *
allocate_async_stream_playback_state(RwMemoryAllocator allocate,
                                     AsyncBlockStream *stream)
{
  AsyncStreamPlaybackStatePrefix *state;

  state = (AsyncStreamPlaybackStatePrefix *)allocate(
    ASYNC_STREAM_PLAYBACK_STATE_BYTES, MEMTYPE_DMA);
  if(state == 0)
    {
      dispose_async_block_stream(stream);
    }
  return state;
}

/* Reconstructed from the original function at 0x000313C4. */
AsyncStreamPlaybackStatePrefix *
create_async_stream_playback_state(RwMemoryAllocator             allocate,
                                   const AsyncStreamOpenRequest *request)
{
  const RwTagArgument *defaults;
  const RwTagArgument *entry;
  AsyncStreamPlaybackStatePrefix *state;
  AsyncBlockStream *stream;
  const RwTagArgument *tags;
  Screen *opened_screen;
  SignedDivisionResult division;
  int io_block_bytes;
  int buffer_group_size;
  int buffer_count;
  int wait_for_buffer;
  int item;
  int allocate_hook;
  int deallocate_hook;

  defaults = stream_open_tags;

  entry = find_stream_tag_argument(
    ASYNC_STREAM_TAG_IO_BLOCK_BYTES, request->tags, defaults);
  io_block_bytes = (int)entry->argument;
  entry = find_stream_tag_argument(
    ASYNC_STREAM_TAG_BUFFER_GROUP_SIZE, request->tags, defaults);
  buffer_group_size = (int)entry->argument;
  entry = find_stream_tag_argument(
    ASYNC_STREAM_TAG_WAIT_FOR_BUFFER, request->tags, defaults);
  wait_for_buffer = (unsigned char)entry->argument;

  if(buffer_group_size <= 0 || request->stream_unit_count <= 0)
    {
      return 0;
    }

  division = signed_divide_with_remainder(
    (unsigned int)buffer_group_size,
    (unsigned int)request->stream_unit_count);
  buffer_count = division.quotient;
  if(division.remainder != 0)
    {
      buffer_count++;
    }

  stream = create_async_block_stream(request->path,
                                     io_block_bytes,
                                     buffer_count,
                                     wait_for_buffer);
  if(stream == 0)
    {
      return 0;
    }

  state = allocate_async_stream_playback_state(allocate, stream);
  if(state == 0)
    {
      return 0;
    }

  memset(state, 0, ASYNC_STREAM_PLAYBACK_STATE_BYTES);
  state->stream_unit_count = request->stream_unit_count;
  state->io_block_bytes = io_block_bytes;
  state->buffer_group_size = buffer_group_size;
  state->active = 0;
  state->repeat = 0;
  state->control_value = 0;
  state->block_stream = stream;
  state->playback_thread_item = CURRENTTASK->t.n_Item;
  state->thread_stack_block = request->thread_stack_block;

  entry = find_stream_tag_argument(
    ASYNC_STREAM_TAG_FIRST_SCREEN_ITEM, request->tags, defaults);
  item = (int)entry->argument;
  if(item == 0)
    {
      state->first_screen_bitmap = 0;
      state->first_screen_item = 0;
    }
  else
    {
      if(item == -1)
        {
          item = OpenItem(get_display_screen_item(0), 0);
        }
      else
        {
          item = OpenItem(item, 0);
        }
      state->first_screen_item = item;
      if(item < 0)
        {
          state->first_screen_item = 0;
          goto cleanup;
        }
      else
        {
          opened_screen = (Screen *)CheckItem(item, NST_GRAPHICS, SCREENNODE);
          if(opened_screen == 0)
            {
              goto cleanup;
            }
          state->first_screen_bitmap = opened_screen->scr_TempBitmap;
        }
    }

  entry = find_stream_tag_argument(
    ASYNC_STREAM_TAG_SECOND_SCREEN_ITEM, request->tags, defaults);
  item = (int)entry->argument;
  if(item == 0)
    {
      state->second_screen_bitmap = 0;
      state->second_screen_item = 0;
    }
  else
    {
      if(item == -1)
        {
          item = OpenItem(get_display_screen_item(0), 0);
        }
      else
        {
          item = OpenItem(item, 0);
        }
      state->second_screen_item = item;
      if(item < 0)
        {
          state->second_screen_item = 0;
          goto cleanup;
        }
      else
        {
          opened_screen = (Screen *)CheckItem(item, NST_GRAPHICS, SCREENNODE);
          if(opened_screen == 0)
            {
              goto cleanup;
            }
          state->second_screen_bitmap = opened_screen->scr_TempBitmap;
        }
    }

  state->tag_overrides[0].tag = 0;
  state->tag_overrides[0].argument = 0;

  tags = request->tags;
  if(tags != 0 && tags->tag != 0)
    {
      do
        {
          switch(tags->tag)
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
                tags);
              break;
            default:
              break;
            }
          tags++;
        } while(tags->tag != 0);
    }

  return state;

cleanup:
  dispose_async_stream_playback_resources(state);
  BF_GetHooks(&allocate_hook, &deallocate_hook);
  ((void (*)(void *, int))deallocate_hook)(
    state, ASYNC_STREAM_PLAYBACK_STATE_BYTES);
  return 0;
}
