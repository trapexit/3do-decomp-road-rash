#include "bss_early_data.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"
#include "path_runtime.h"

#define FRONT_END_RESOURCE_PATH_LENGTH 0x34
#define FRONT_END_CEL_HIERARCHY_COUNT 0x3F4

void enable_movie_playback(void);
int start_next_background_audio_stream(void);
int start_front_end_menu_audio(void);
int stop_background_audio_stream(void);
int stop_front_end_menu_audio(void);
int RSRC_DisposeFileResources(MemoryHandle *resource_file);

int
dispose_front_end_assets(void)
{
  int result;

  result = stop_background_audio_stream();
  if(background_audio_globals.stream_state != 0)
    {
      return result < 0 ? result : -1;
    }
  result = stop_front_end_menu_audio();
  if(front_end_asset_state.menu_audio_item != -1)
    {
      return result < 0 ? result : -1;
    }
  if(front_end_asset_state.background_audio_resource != 0)
    {
      result = RSRC_DisposeFileResources(
        front_end_asset_state.background_audio_resource);
      if(result != 0)
        {
          return result < 0 ? result : -1;
        }
      front_end_asset_state.background_audio_resource = 0;
    }
  if(front_end_asset_state.font_resource != 0)
    {
      unload_text_font((int)front_end_asset_state.font_resource);
      front_end_asset_state.font_resource = 0;
    }
  return 0;
}

int
initialize_front_end_assets(void)
{
  const char *base_path;
  int font;
  void *audio_resource;
  int index;
  int result;
  char path[FRONT_END_RESOURCE_PATH_LENGTH];
  FrontEndCelHierarchyEntry *entry;

  result = dispose_front_end_assets();
  if(result < 0)
    {
      return result;
    }
  if(runtime_feature_flags.streaming_audio_enabled.value != 0)
    {
      enable_movie_playback();
    }
  base_path = resource_root_path;
  if(copy_path_with_capacity(path, sizeof(path), base_path) < 0 ||
     append_path_with_capacity(path, sizeof(path), "Front End Font") < 0)
    {
      return PATH_RUNTIME_ERROR_TOO_LONG;
    }
  font = load_text_font(path);
  if(font == 0)
    {
      return -1;
    }
  front_end_asset_state.font_resource = (void *)font;
  select_text_font(font);
  set_text_shadow_offset(1, 1);

  for(index = 0; index < FRONT_END_CEL_HIERARCHY_COUNT; index++)
    {
      entry = &gFrontEndCelHierarchy[index];
      entry->resource_id = index;
      entry->parent_index = -1;
      entry->child_selector = 0;
    }
  if(copy_path_with_capacity(path, sizeof(path), base_path) < 0 ||
     append_path_with_capacity(path, sizeof(path),
                               "Streams/bgaudio/RashIF.RSRC") < 0)
    {
      result = PATH_RUNTIME_ERROR_TOO_LONG;
      goto fail;
    }
  audio_resource = RSRC_OpenFile(path, 0);
  if(audio_resource == 0)
    {
      result = -1;
      goto fail;
    }
  front_end_asset_state.background_audio_resource = audio_resource;
  if(race_mode_state.outcome_mode == 0)
    {
      result = start_next_background_audio_stream();
      if(result < 0)
        {
          goto fail;
        }
      result = start_front_end_menu_audio();
      if(result != 0)
        {
          if(result > 0)
            {
              result = -1;
            }
          goto fail;
        }
    }
  return 0;

fail:
  dispose_front_end_assets();
  return result;
}

#undef FRONT_END_RESOURCE_PATH_LENGTH
#undef FRONT_END_CEL_HIERARCHY_COUNT
