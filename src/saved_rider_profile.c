#include "saved_rider_profile.h"

int
saved_rider_profile_is_valid(const SavedRiderProfile *profile,
                             int                      byte_count)
{
  if(profile == 0 || byte_count != SAVED_PROFILE_RECORD_SIZE)
    {
      return 0;
    }
  if(profile->signature != SAVED_PROFILE_SIGNATURE)
    {
      return 0;
    }
  if(profile->music_enabled > 1 || profile->engine_audio_enabled > 1)
    {
      return 0;
    }
  if(profile->rider_type < 0 ||
     profile->rider_type >= SAVED_PROFILE_RIDER_TYPE_COUNT)
    {
      return 0;
    }
  if(profile->level < 0 ||
     profile->level >= SAVED_PROFILE_PROGRESSION_LEVEL_COUNT)
    {
      return 0;
    }
  if(profile->current_variant < 0 ||
     profile->current_variant >= SAVED_PROFILE_VARIANT_COUNT)
    {
      return 0;
    }
  if(profile->rider_style < 0 ||
     profile->rider_style >= SAVED_PROFILE_BIKE_STYLE_COUNT)
    {
      return 0;
    }
  if((profile->completed_courses &
      ~SAVED_PROFILE_COMPLETED_COURSE_MASK) != 0)
    {
      return 0;
    }
  return 1;
}
