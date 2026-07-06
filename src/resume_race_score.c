/* Guard at 0x0001013C reconstructed from the original function at 0x000106EC. */



#include "rw_semantic_data.h"

extern
int
resume_score(void);

int
resume_race_score(void)
{
  if(game_audio_state.initialized != 0)
    {
      return resume_score();
    }
  return 0;
}
