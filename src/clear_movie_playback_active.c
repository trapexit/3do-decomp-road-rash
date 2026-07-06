/* Reconstructed from the original function at 0x00027BBC. */



#include "rw_semantic_data.h"

void
clear_movie_playback_active(void)
{
  if(movie_playback_control.active != 0)
    {
      movie_playback_control.active = 0;
    }
}
