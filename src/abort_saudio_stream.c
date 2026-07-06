#include "saudio_subscriber_runtime.h"

int
abort_saudio_stream(SAudioContext *context)
{
  return close_saudio_stream_channels(context);
}
