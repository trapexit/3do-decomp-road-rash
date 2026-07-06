#include "semaphore.h"

#include "family_resource_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

static Item family_resource_render_semaphore = -1;

unsigned int family_resource_generations[FAMILY_GROUP_COUNT];
int family_resource_failure_codes[FAMILY_GROUP_COUNT];
int family_resource_failure_values[FAMILY_GROUP_COUNT];
volatile int family_resource_worker_request_pending;
volatile int family_resource_worker_startup_status;
volatile int family_resource_worker_startup_signal;
volatile int family_resource_worker_startup_task_item;
volatile int family_resource_worker_shutdown_requested;

int
initialize_family_resource_render_barrier(void)
{
  if(family_resource_render_semaphore >= 0)
    {
      return 0;
    }
  family_resource_render_semaphore =
    CreateSemaphore("FamilyRender", 0);
  if(family_resource_render_semaphore <= 0)
    {
      family_resource_render_semaphore = -1;
      return 0;
    }
  return 1;
}


int
dispose_family_resource_render_barrier(void)
{
  int result;

  if(family_resource_render_semaphore >= 0)
    {
      result = DeleteItem(family_resource_render_semaphore);
      if(result < 0)
        {
          return result;
        }
      family_resource_render_semaphore = -1;
    }
  return 0;
}


int
try_acquire_family_resource_render_barrier(void)
{
  if(family_resource_render_semaphore < 0)
    {
      return 0;
    }
  return LockSemaphore(family_resource_render_semaphore, 0) > 0;
}


int
acquire_family_resource_render_barrier(void)
{
  if(family_resource_render_semaphore < 0)
    {
      return 0;
    }
  return LockSemaphore(family_resource_render_semaphore, SEM_WAIT) > 0;
}


void
release_family_resource_render_barrier(void)
{
  if(family_resource_render_semaphore >= 0)
    {
      UnlockSemaphore(family_resource_render_semaphore);
    }
}


void *
load_and_fixup_family_resource_group(const FamilyResourceWorkerRequest *request)
{
  void *group_destination;
  void *load_result;
  FamilyResourceTable *table;
  unsigned int generation;
  int family_id;
  int group_index;
  int resource_bytes;

  family_id = request->family_id;
  group_index = request->group_index;
  family_resource_globals.resident_family_ids[group_index] = -family_id;
  family_resource_failure_codes[group_index] =
    FAMILY_RESOURCE_LOAD_FAILURE_NONE;
  family_resource_failure_values[group_index] = 0;
  family_resource_loaded_bytes[group_index] = 0U;
  group_destination = family_resource_globals.group_tables[group_index];
  table = (FamilyResourceTable *)group_destination;
  if(table != 0)
    {
      table->entry_count = 0;
    }
  resource_bytes = RSRC_GetResourceSize(
    FAMILY_RESOURCE_TYPE_TAG, family_id);
  family_resource_failure_values[group_index] = resource_bytes;
  if(resource_bytes == 0)
    {
      family_resource_failure_codes[group_index] =
        FAMILY_RESOURCE_LOAD_FAILURE_NOT_FOUND;
      return 0;
    }
  if(resource_bytes < 0 ||
     resource_bytes > family_resource_globals.table_bytes)
    {
      family_resource_failure_codes[group_index] =
        FAMILY_RESOURCE_LOAD_FAILURE_SIZE;
      return 0;
    }
  load_result = load_family_resource_chunk_into_group(
    family_id, group_destination);
  if(load_result == 0)
    {
      family_resource_failure_codes[group_index] =
        FAMILY_RESOURCE_LOAD_FAILURE_STUFF;
      if(table != 0)
        {
          table->entry_count = 0;
        }
    }
  if(load_result != 0)
    {
      relocate_family_resource_value_tree(group_destination);
      family_resource_loaded_bytes[group_index] =
        (unsigned int)resource_bytes;
      generation = family_resource_generations[group_index] + 1U;
      if(generation == 0)
        {
          generation = 1;
        }
      family_resource_generations[group_index] = generation;
      family_resource_globals.resident_family_ids[group_index] =
        family_id;
    }
  return load_result;
}
