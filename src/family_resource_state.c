#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

unsigned int family_resource_loaded_bytes[FAMILY_GROUP_COUNT];

int
family_resource_worker_is_ready(void)
{
  return family_resource_globals.worker_task_item > 0 &&
         family_resource_globals.wake_signal != 0 &&
         family_resource_globals.worker_running != 0 &&
         family_resource_worker_startup_status ==
         FAMILY_RESOURCE_WORKER_READY;
}


void
mark_family_resource_group_unavailable(int group_index,
                                       int family_id)
{
  FamilyResourceTable *table;

  family_resource_globals.resident_family_ids[group_index] = -family_id;
  family_resource_loaded_bytes[group_index] = 0U;
  table = (FamilyResourceTable *)
          family_resource_globals.group_tables[group_index];
  if(table != 0)
    {
      table->entry_count = 0;
    }
}
