/* Reconstructed from the original function at 0x00027BA0. */



#include "rw_semantic_data.h"

void
enable_movie_playback(void)
{
  if(movie_playback_control.active == 0)
    {
      movie_playback_control.active = 1;
    }
}
