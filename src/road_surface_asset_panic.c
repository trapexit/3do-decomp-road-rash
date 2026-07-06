#include "debug.h"
#include "stdlib.h"

#include "bss_early_data.h"
#include "family_resource_runtime.h"
#include "road_surface_asset_panic.h"
#include "rw_semantic_data.h"

static const char *
road_surface_asset_failure_name(RoadSurfaceAssetFailure failure)
{
  switch(failure)
    {
      case ROAD_SURFACE_ASSET_FAMILY_UNAVAILABLE:
        return "family-unavailable";
      case ROAD_SURFACE_ASSET_ENTRY_MISSING:
        return "entry-missing";
      case ROAD_SURFACE_ASSET_DIMENSION_SELECTION_FAILED:
        return "dimension-selection-failed";
    }
  return "unknown";
}

static const char *
family_resource_load_failure_name(int failure)
{
  switch(failure)
    {
      case FAMILY_RESOURCE_LOAD_FAILURE_NONE:
        return "none";
      case FAMILY_RESOURCE_LOAD_FAILURE_NOT_FOUND:
        return "resource-not-found";
      case FAMILY_RESOURCE_LOAD_FAILURE_SEEK:
        return "seek-failed";
      case FAMILY_RESOURCE_LOAD_FAILURE_SIZE:
        return "invalid-resource-size";
      case FAMILY_RESOURCE_LOAD_FAILURE_STUFF:
        return "resource-stuff-failed";
    }
  return "unknown";
}


void
panic_missing_road_surface_asset(
  const RoadRenderSide     *road_side,
  RoadSide                  side,
  int                       selector,
  int                       family_index,
  int                       child_index,
  int                       width,
  int                       height,
  RoadSurfaceAssetFailure   failure)
{
  const RoadSegmentLaneRuntime *lane;
  FamilyResourceTable *table;
  unsigned int generation;
  unsigned int loaded_bytes;
  int depth;
  int entry_count;
  int geometry_mode;
  int family_id;
  int failure_code;
  int failure_value;
  int group_index;
  int group_lock;
  int resident_family;

  group_index = FAMILY_RESOURCE_GROUP_INDEX(selector);
  resident_family = 0;
  family_id = 0;
  failure_code = FAMILY_RESOURCE_LOAD_FAILURE_NONE;
  failure_value = 0;
  generation = 0;
  loaded_bytes = 0;
  entry_count = 0;
  group_lock = FAMILY_RESOURCE_LOCK_FREE;
  if(group_index >= 0 &&
     group_index < family_resource_globals.group_count)
    {
      resident_family =
        family_resource_globals.resident_family_ids[group_index];
      family_id = resident_family;
      if(family_id < 0)
        {
          family_id = -family_id;
        }
      group_lock = family_resource_globals.group_locks[group_index];
      failure_code = family_resource_failure_codes[group_index];
      failure_value = family_resource_failure_values[group_index];
      generation = family_resource_generations[group_index];
      loaded_bytes = family_resource_loaded_bytes[group_index];
      table = (FamilyResourceTable *)
              family_resource_globals.group_tables[group_index];
      if(table != 0)
        {
          entry_count = table->entry_count;
        }
    }

  lane = 0;
  depth = -1;
  geometry_mode = -1;
  if(road_side != 0)
    {
      lane = road_side->lane;
      if(road_side->owner_node != 0)
        {
          depth = road_side->owner_node->depth;
        }
    }
  if(lane != 0)
    {
      geometry_mode = lane->geometry_mode;
    }

  kprintf("PANIC: missing road surface asset reason=%s\n",
          road_surface_asset_failure_name(failure));
  kprintf("asset=Families.Rsrc:FAM[%ld]/table[%ld]/entry[%ld]\n",
          family_id, family_index,
          FAMILY_RESOURCE_ENTRY_INDEX(selector));
  kprintf("selector=%ld group=%ld child=%ld\n",
          selector, group_index, child_index);
  kprintf("resident_state=%ld generation=%lu loaded_bytes=%lu\n",
          resident_family, generation, loaded_bytes);
  kprintf("table_entries=%ld group_lock=%ld last_seek=%ld\n",
          entry_count, group_lock,
          family_resource_globals.last_seek_result);
  kprintf("load_failure=%s code=%ld value=%ld\n",
          family_resource_load_failure_name(failure_code),
          failure_code, failure_value);
  kprintf("request_pending=%ld worker_status=%ld worker_running=%ld\n",
          family_resource_worker_request_pending,
          family_resource_worker_startup_status,
          family_resource_globals.worker_running);
  kprintf("side=%ld depth=%ld geometry=%ld\n",
          side, depth, geometry_mode);
  kprintf("width=%ld height=%ld\n", width, height);
  exit(1);
}
