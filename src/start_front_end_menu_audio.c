/* Reconstructed from the original function at 0x00009288. */



#include "aiff_sample_runtime.h"
#include "rw_semantic_data.h"

#define AIFF_RESOURCE_TYPE 0x41494646
#define FRONT_END_MENU_AUDIO_RESOURCE_INDEX 1
#define RESOURCE_LOAD_PINNED 8
#define RESOURCE_NODE_BUSY 0x01000000

int *
RSRC_LoadResource(int resource_type,
                  int resource_index,
                  int flags);

int
start_front_end_menu_audio(void)
{
  int *node;
  int result;

  if(front_end_asset_state.menu_audio_item != -1)
    {
      return 0;
    }
  if(runtime_feature_flags.game_audio_enabled.value == 0)
    {
      return 0;
    }

  node = RSRC_LoadResource(AIFF_RESOURCE_TYPE,
                           FRONT_END_MENU_AUDIO_RESOURCE_INDEX,
                           RESOURCE_LOAD_PINNED);
  if(node == 0)
    {
      return *(int *)(int)&(resource_system_last_error);
    }

  node[1] |= RESOURCE_NODE_BUSY;
  result = node[0];
  if(result == 0)
    {
      return *(int *)(int)&(memory_system_last_error);
    }

  result = load_aiff_sample_image((void *)result, 0);
  front_end_asset_state.menu_audio_item = result >= 0 ? result : -1;
  if(result >= 0)
    {
      return 0;
    }
  return result;
}


#undef AIFF_RESOURCE_TYPE
#undef FRONT_END_MENU_AUDIO_RESOURCE_INDEX
#undef RESOURCE_LOAD_PINNED
#undef RESOURCE_NODE_BUSY
