/* Reconstructed from the original function at 0x000334FC. */



#include "async_stream_runtime.h"
#include "rw_semantic_data.h"

int
initialize_default_music_context(void);

int
stop_background_audio_stream(void)
{
  AsyncStreamPlaybackStatePrefix *state;
  int stop_accepted;
  int result;
  int restore_result;

  state = (AsyncStreamPlaybackStatePrefix *)
    background_audio_globals.stream_state;
  if(state == 0)
    {
      return 0;
    }

  result = stop_async_stream(state, &stop_accepted);
  if(stop_accepted == 0)
    {
      return result;
    }

  background_audio_globals.stream_state = 0;
  restore_result = initialize_default_music_context();
  if(result < 0)
    {
      return result;
    }
  return restore_result;
}
