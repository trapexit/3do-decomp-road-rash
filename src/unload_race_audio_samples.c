#include "race_audio_runtime.h"
#include "rw_semantic_data.h"

int
unload_sample_image(int sample_item,
                    int release_sample_data);
int
RSRC_DisposeFileResources(void *resource_file);

int
unload_race_audio_samples(void)
{
  int sample_index;
  int result;
  int release_result;

  result = 0;

  for(sample_index = 0;
      sample_index < GAME_SOUND_RESOURCE_COUNT;
      sample_index++)
    {
      if(game_audio_state.sound_sample_items[sample_index] >= 0)
        {
          release_result = unload_sample_image(
            game_audio_state.sound_sample_items[sample_index], 0);
          if(release_result < 0)
            {
              if(result >= 0)
                {
                  result = release_result;
                }
            }
          else
            {
              game_audio_state.sound_sample_items[sample_index] = -1;
            }
        }
    }

  if(result < 0)
    {
      return result;
    }
  if(game_audio_state.sound_resource_file != 0)
    {
      result = RSRC_DisposeFileResources(game_audio_state.sound_resource_file);
      if(result != 0)
        {
          return result < 0 ? result : -1;
        }
    }
  game_audio_state.sound_resource_file = 0;
  return 0;
}
