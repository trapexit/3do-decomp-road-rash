#include "async_stream_runtime.h"

/* Reconstructed from the original function at 0x00033018. */
int
get_async_block_stream_status(const AsyncBlockStream *stream)
{
  if(stream != 0)
    {
      return stream->last_error;
    }
  return -1;
}
