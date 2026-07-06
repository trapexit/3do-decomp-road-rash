
#include "application_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

void
clear_rider_animation_resource_pairs(void);
int
RSRC_DisposeFileResources(void *resource_file);

void
unload_rash_options_resources(void)
{
  ResourceCatalogEntry *entry;

  entry = rash_options_catalog;
  while(entry->destination != 0)
    {
      *(void **)entry->destination = 0;
      entry->node = 0;
      entry->resource = 0;
      entry++;
    }

  clear_road_particle_resources();
  clear_rider_animation_resource_pairs();
  if(resource_catalog_handles.rash_options_resource != 0)
    {
      RSRC_DisposeFileResources(
        resource_catalog_handles.rash_options_resource);
      resource_catalog_handles.rash_options_resource = 0;
    }
}
