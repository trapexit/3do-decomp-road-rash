#include "stdio.h"
/* Reconstructed from the original function at 0x00033700. */

#include "async_stream_runtime.h"
#include "event.h"
#include "rw_semantic_data.h"

int
start_next_background_audio_stream(void);
int
stop_background_audio_stream(void);
void
adjust_background_audio_gain_from_input(int flags);

int
service_background_audio(int flags)
{
  BackgroundAudioGlobals *state;
  int result;
  int buffered;

  state = &background_audio_globals;
  if(state->stream_state != 0)
    {
      result = poll_async_stream_completion(
        (AsyncStreamPlaybackStatePrefix *)state->stream_state, 0);
      if(result < 0)
        {
          stop_background_audio_stream();
          return result;
        }
      buffered = 0;
      if(result != 0)
        {
          buffered = state->enabled;
        }
      if(result != 0 && buffered != 0)
        {
          result = stop_background_audio_stream();
          if(result < 0 || background_audio_globals.stream_state != 0)
            {
              printf("BG_ROTATE_DECLINED stop=%d\n", result);
              return result;
            }
          result = start_next_background_audio_stream();
          if(result != 0)
            {
              printf("BG_ROTATE_START_FAIL start=%d\n", result);
            }
          return result;
        }
    }

  if((flags & (ControlLeftShift | ControlRightShift)) != 0)
    {
      adjust_background_audio_gain_from_input(flags);
    }
  return 0;
}
