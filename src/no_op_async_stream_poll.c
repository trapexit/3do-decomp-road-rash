#include "async_stream_runtime.h"

int
no_op_async_stream_poll(AsyncStreamPlaybackStatePrefix *stream_state)
{
  return 0;
}
