/* Reconstructed from the original function at 0x00027C44. */

#include "control_subscriber_runtime.h"
#include "cpak_subscriber_runtime.h"
#include "data_stream_runtime.h"
#include "saudio_subscriber_runtime.h"
#include "stream_acquisition_runtime.h"
#include "movie_playback_runtime.h"

int
dispose_movie_subscribers(void)
{
  int status;

  status = release_movie_playback_resources();
  if(status < 0)
    {
      return status;
    }
  close_control_subscriber_pool();
  dispose_saudio_subscriber_context_pool();
  close_cpak_subscriber_pools();
  CloseDataStreaming();
  return dispose_stream_acquisition_context_pool();
}
