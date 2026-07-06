/* Reconstructed from the original function at 0x00027BD4. */

#include "control_subscriber_runtime.h"
#include "cpak_subscriber_runtime.h"
#include "data_stream_runtime.h"
#include "saudio_subscriber_runtime.h"
#include "stream_acquisition_runtime.h"
#include "movie_playback_runtime.h"

void
clear_movie_playback_active(void);

int
initialize_movie_subscribers(void)
{
  int status;

  status = dispose_movie_subscribers();
  if(status < 0)
    {
      return status;
    }

  status = initialize_stream_acquisition_context_pool(1);
  if(status < 0)
    {
      goto failure;
    }

  status = InitDataStreaming(1);
  if(status < 0)
    {
      goto failure;
    }

  status = initialize_cpak_subscriber_pools();
  if(status < 0)
    {
      goto failure;
    }

  status = initialize_saudio_subscriber_context_pool();
  if(status < 0)
    {
      goto failure;
    }

  status = initialize_control_subscriber_pool();
  if(status >= 0)
    {
      return 0;
    }

failure:
  clear_movie_playback_active();
  dispose_movie_subscribers();
  return status;
}
