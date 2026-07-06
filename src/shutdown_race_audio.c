#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
unload_sample_image(int sample_item,
                    int release_sample_data);
int
initialize_music_context_from_tags(int tag_list);

int
shutdown_race_audio(void)
{
  int item;
  int result;

  game_audio_state.initialized = 0;

  result = release_race_audio_voice_resources();
  if(result < 0)
    {
      return result;
    }
  result = unload_race_audio_samples();
  if(result < 0)
    {
      return result;
    }

  item = game_audio_state.transient_sample_item;
  if(item > 0)
    {
      /* Dynamic family samples are copied before the reusable group can
         be replaced, so release that owned sample data on shutdown. */
      result = unload_sample_image(item, 1);
      if(result < 0)
        {
          return result;
        }
    }

  game_audio_state.transient_sample_item = 0;
  return initialize_music_context_from_tags(0);
}
