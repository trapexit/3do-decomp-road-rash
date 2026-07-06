#include "rw_semantic_data.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x00020F98. */

int
find_active_level_finish_sample(const RoadSectionResource *resource)
{
  int entry_count;
  const RoadSectionEntry *entry;
  int profile_index;
  RiderProfile *profile;

  if(resource == 0)
    {
      return 0;
    }

  entry = resource->entries;
  entry_count = resource->entry_count;
  if(entry_count > 0)
    {
      profile_index = race_mode_state.active_player_index;
      profile = &player_profiles[profile_index];
      do
        {
          if(entry->kind == ROAD_SECTION_ENTRY_KIND_LEVEL_GATE &&
             entry->selector - 1 == profile->level)
            {
              return entry->start_sample;
            }
          entry++;
          entry_count--;
        } while(entry_count > 0);
    }

  return 0;
}
