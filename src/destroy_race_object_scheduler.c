#include "intrusive_list.h"
#include "rw_semantic_data.h"

void
destroy_race_object_scheduler(void)
{
  if(race_object_scheduler.object_list != 0)
    {
      dispose_intrusive_list_storage_if_empty(
        (IntrusiveListStorage *)race_object_scheduler.object_list);
    }

  race_object_scheduler.object_list = 0;
}
