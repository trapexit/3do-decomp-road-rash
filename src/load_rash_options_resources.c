#include "string.h"

#include "application_runtime.h"
#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"
#include "rider_animation_resources.h"
#include "rw_semantic_data.h"

enum
{
  PREPARED_RESOURCE_FILE_OPTIONS = 0x1000
};

MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options);

void
load_rash_options_resources(void)
{
  char path[FAMILY_RESOURCE_PATH_BYTES];
  int index;
  ResourceCatalogEntry *entry;
  MemoryHandle *node;

  strcpy(path, (char *)(int)&(resource_root_path));
  strcat(path, "RashOpt.RSRC");

  resource_catalog_handles.rash_options_resource =
    RSRC_LoadFile(path, PREPARED_RESOURCE_FILE_OPTIONS);
  if(resource_catalog_handles.rash_options_resource == 0)
    {
      abort_application_with_resource_error(resource_system_last_error);
    }

  load_rider_animation_resources();
  load_road_particle_resources();

  index = 0;
  entry = rash_options_catalog;
  while(entry->destination != 0)
    {
      if(entry->resource == 0)
        {
          node = RSRC_LoadResource(
            entry->chunk_type, entry->resource_index + 1,
            RESOURCE_LOAD_OPTION_KEEP_BUSY);
          entry->node = node;
          if(node == 0)
            {
              abort_application_with_resource_error(
                resource_system_last_error);
            }
          else
            {
              node->flags |= MEM_HANDLE_FLAG_BUSY;
              entry->resource = node->data;
              if(entry->resource == 0)
                {
                  abort_application_with_resource_error(
                    memory_system_last_error);
                }
            }
        }

      index++;
      entry++;
    }

  index = 0;
  entry = rash_options_catalog;
  while(entry->destination != 0)
    {
      if(entry->chunk_type == CANS_RESOURCE_TYPE_CEL)
        {
          entry->resource = parse_cans_cel_chunk_stream(
            (CansChunkHeader *)entry->resource, 0);
          if(entry->resource == 0)
            {
              abort_application_with_resource_error(index + 1);
            }
        }

      *(void **)entry->destination = entry->resource;
      index++;
      entry++;
    }
}
