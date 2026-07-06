#include "bss_early_data.h"
#include "disk_file_runtime.h"
#include "rw_semantic_data.h"
#include "saved_rider_profile.h"

/* Reconstructed from the original function at 0x00009B80. */



void
select_rider_opponents(int player_index);
#include "memory_manager_runtime.h"

int
load_rider_profile_slot(int profile_slot,
                        int player_index)
{
  char path[SAVED_PROFILE_PATH_BUFFER_LENGTH] = SAVED_PROFILE_PATH;
  char *loaded_data;
  SavedRiderProfile *saved_profile;
  RiderProfile *profile;
  int result;
  int row;
  int field;
  int rider_type;
  int loaded_capacity;

  path[SAVED_PROFILE_PATH_SLOT_OFFSET] = (char)('0' + profile_slot);

  loaded_data = 0;
  loaded_capacity = 0;
  result = load_disk_file(path, &loaded_data, &loaded_capacity);
  if(saved_rider_profile_is_valid(
       (SavedRiderProfile *)loaded_data, result) != 0)
    {
      saved_profile = (SavedRiderProfile *)loaded_data;
      race_mode_state.game_mode = 0;
      race_mode_state.music_enabled = saved_profile->music_enabled;
      race_mode_state.engine_audio_enabled =
        saved_profile->engine_audio_enabled;

      rider_type = saved_profile->rider_type;
      profile = &player_profiles[player_index];
      profile->rider_type = (signed char)rider_type;
      profile->current_variant = saved_profile->current_variant;
      profile->level = saved_profile->level;
      profile->completed_courses = saved_profile->completed_courses;
      profile->balance = saved_profile->balance;
      profile->rider_style = saved_profile->rider_style;

      for(row = 0; row < SAVED_PROFILE_PROGRESSION_ROW_COUNT; row++)
        {
          for(field = 0;
              field < SAVED_PROFILE_PROGRESSION_VALUE_COUNT; field++)
            {
              gRiderProgressionProfiles[player_index][row]
              .words[rider_type *
                     SAVED_PROFILE_PROGRESSION_VALUE_COUNT + field] =
                saved_profile->progression[row].values[field];
            }
        }

      result = 0;
      select_rider_opponents((signed char)player_index);
    }

  if(loaded_data != 0)
    {
      MEM_DisposePointer(loaded_data);
    }

  return result;
}
