
#include "audio_manager_voice.h"
#include "rw_semantic_data.h"

#define FRONT_END_MENU_AUDIO_GAIN 0x5FFF
#define FRONT_END_MENU_AUDIO_PAN 0x40
#define FRONT_END_MENU_AUDIO_SORT_KEY 0x64

int
unload_sample_image(int sample_item,
                    int release_data);

int
stop_front_end_menu_audio(void)
{
  int item;
  int result;

  item = front_end_asset_state.menu_audio_item;
  if(item >= 0)
    {
      result = unload_sample_image(item, 0);
      if(result < 0)
        {
          return result;
        }
      front_end_asset_state.menu_audio_item = -1;
    }

  return -1;
}


int
play_front_end_menu_audio(void)
{
  int item;

  item = front_end_asset_state.menu_audio_item;
  if(item == -1)
    {
      return 0;
    }

  return play_audio_manager_sample(item, -1,
                                   FRONT_END_MENU_AUDIO_GAIN,
                                   FRONT_END_MENU_AUDIO_PAN,
                                   FRONT_END_MENU_AUDIO_SORT_KEY);
}


#undef FRONT_END_MENU_AUDIO_GAIN
#undef FRONT_END_MENU_AUDIO_PAN
#undef FRONT_END_MENU_AUDIO_SORT_KEY
