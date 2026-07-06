#include "stddef.h"

#include "async_stream_runtime.h"
#include "audio.h"
#include "item.h"
/* Reconstructed from the original function at 0x000311D8. */

typedef char AsyncStreamCompletionSlotSizeIs0x10[
    sizeof(AsyncStreamCompletionSlot) == 0x10 ? 1 : -1];
typedef char AsyncStreamCompletionSlotsOffsetIs0xEC[
    offsetof(AsyncStreamCompletionStateView, slots) == 0xEC ? 1 : -1];

int
poll_async_stream_completion(AsyncStreamPlaybackStatePrefix *stream_state,
                             int                             wait_for_completion)
{
  AsyncStreamCompletionStateView *state;
  int wait;
  Item timer_cue_item;
  int slot_index;
  int has_pending_item;
  int now;

  int result;
  state = (AsyncStreamCompletionStateView *)stream_state;
  wait = (unsigned char)wait_for_completion;
  timer_cue_item = 0;

  if(state == 0)
    {
      return 1;
    }

  while(state->prefix.active != 0 && wait != 0)
    {
      has_pending_item = 0;
      for(slot_index = 0;
          slot_index < ASYNC_STREAM_COMPLETION_SLOT_COUNT;
          slot_index++)
        {
          if(state->slots[slot_index].pending_item != -1)
            {
              has_pending_item = 1;
              break;
            }
        }

      if(has_pending_item)
        {
          no_op_async_stream_poll(&state->prefix);
          if(timer_cue_item == 0)
            {
              timer_cue_item = CreateSizedItem(
                MKNODEID(AUDIONODE, AUDIO_CUE_NODE), 0, 0);
              if(timer_cue_item < 0)
                {
                  return timer_cue_item;
                }
            }
          now = GetAudioTime();
          result = SleepUntilTime(timer_cue_item, now + 1);
          if(result < 0)
            {
              DeleteItem(timer_cue_item);
              return result;
            }
        }
      else
        {
          result = LockItem(state->prefix.playback_semaphore_item, 1);
          if(result < 0)
            {
              if(timer_cue_item > 0)
                {
                  DeleteItem(timer_cue_item);
                }
              return result;
            }
          UnlockItem(state->prefix.playback_semaphore_item);
        }
    }

  if(timer_cue_item != 0)
    {
      DeleteItem(timer_cue_item);
    }
  if(state->prefix.active == 0 &&
     state->prefix.block_stream != 0 &&
     state->prefix.block_stream->last_error < 0)
    {
      return state->prefix.block_stream->last_error;
    }


  return state->prefix.active == 0 ? 1 : 0;
}
