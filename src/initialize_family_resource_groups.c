#include "limits.h"
#include "string.h"

#include "family_resource_runtime.h"
#include "memory_manager_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"
#include "track_traversal_runtime.h"

/* Reconstructed from the original function at 0x00019E54. */

int
initialize_family_resource_groups(RoadFamilyScheduleResource *schedule_resource)
{
  char path[FAMILY_RESOURCE_PATH_BYTES];
  unsigned char *group_table;
  int group_count;
  int group_index;
  int result;
  int table_size_kib;

  family_resource_globals.request_write_index = 0;
  family_resource_globals.request_read_index = 0;
  family_resource_request_count = 0;
  family_resource_worker_request_pending = 0;
  family_resource_worker_startup_status =
    FAMILY_RESOURCE_WORKER_NOT_READY;
  family_resource_worker_startup_signal = 0;
  family_resource_worker_startup_task_item = 0;
  family_resource_worker_shutdown_requested = 0;
  memset(family_resource_generations, 0,
         sizeof(unsigned int) * FAMILY_GROUP_COUNT);
  memset(family_resource_loaded_bytes, 0,
         sizeof(unsigned int) * FAMILY_GROUP_COUNT);
  memset(family_resource_failure_codes, 0,
         sizeof(int) * FAMILY_GROUP_COUNT);
  memset(family_resource_failure_values, 0,
         sizeof(int) * FAMILY_GROUP_COUNT);
  segment_repair_globals.repair_config = schedule_resource;

  if(schedule_resource == 0)
    {
      family_resource_globals.group_count = 0;
      family_resource_globals.table_bytes = 0;
      return 0;
    }

  group_count = schedule_resource->family_group_count;
  table_size_kib = schedule_resource->family_table_size_kib;
  family_resource_globals.group_count = group_count;
  if(group_count <= 0 || table_size_kib <= 0 ||
     table_size_kib > (INT_MAX >> FAMILY_RESOURCE_TABLE_KIB_SHIFT))
    {
      family_resource_globals.group_count = 0;
      family_resource_globals.table_bytes = 0;
      return 0;
    }
  family_resource_globals.table_bytes =
    table_size_kib << FAMILY_RESOURCE_TABLE_KIB_SHIFT;
  if(group_count > FAMILY_GROUP_COUNT)
    {
      group_count = FAMILY_GROUP_COUNT;
      family_resource_globals.group_count = group_count;
    }

  for(group_index = 0;
      group_index < family_resource_globals.group_count;
      group_index++)
    {
      family_resource_globals.resident_family_ids[group_index] = -1;
      release_family_resource_lock(
        &family_resource_globals.group_locks[group_index]);

      /* A failed allocation must not expose a pointer retained from an
         earlier runtime.  Restrict rollback to the tables allocated by
         this attempt. */
      family_resource_globals.group_tables[group_index] = 0;
      group_table = (unsigned char *)MEM_NewPointer(
        family_resource_globals.table_bytes, MEMORY_REQUEST_ANY,
        MEM_POINTER_OPTIONS_DEFAULT);
      family_resource_globals.group_tables[group_index] = group_table;
      if(group_table == 0)
        {
          family_resource_globals.group_count = group_index;
          return 0;
        }
      ((FamilyResourceTable *)group_table)->entry_count = 0;
    }

  strcpy(path, resource_root_path);
  strcat(path, "Families.Rsrc");
  result = (int)RSRC_OpenFile(path, FAMILY_RESOURCE_FILE_LOAD_FLAGS);
  family_resource_globals.resource_file_item = result;

  return result;
}
