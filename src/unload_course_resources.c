#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

int
RSRC_DisposeFileResources(void *resource_file);

void
unload_course_resources(void)
{
  ResourceCatalogEntry *entry;

  entry = course_catalog;

  while(entry->destination != 0)
    {
      *(void **)entry->destination = 0;
      entry->node = 0;
      entry->resource = 0;
      entry++;
    }

  if(resource_catalog_handles.course_resource != 0)
    {
      RSRC_DisposeFileResources(resource_catalog_handles.course_resource);
      resource_catalog_handles.course_resource = 0;
    }
}
