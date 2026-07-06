#include "intrusive_list.h"
#include "rw_semantic_data.h"

enum RaceObjectSchedulerConstant
{
  DEFAULT_RACE_CAMERA_HEIGHT = 150
};

void
reset_racer_finish_tracking(void);

void
initialize_race_object_scheduler(void)
{
  if(race_object_scheduler.object_list == 0)
    {
      race_object_scheduler.camera_height_offset =
        DEFAULT_RACE_CAMERA_HEIGHT;
      race_object_scheduler.camera_follow_distance = 0;
      race_object_scheduler.removal_pending = 0;
      race_object_scheduler.object_list =
        (IntrusiveListHeader *)initialize_intrusive_list_storage(0);
      race_object_scheduler.first_visible_object = 0;
      race_object_scheduler.last_visible_object = 0;
      race_object_scheduler.object_update_tick = 0;
    }

  reset_racer_finish_tracking();
}
