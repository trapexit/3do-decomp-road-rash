#include "string.h"

#include "application_runtime.h"
#include "family_resource_runtime.h"
#include "front_end_cel_runtime.h"
#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"

enum
{
  PREPARED_COURSE_RESOURCE_FILE_OPTIONS = 0x1000
};

MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options);

SegmentTopologyNode *
load_course_resources(void)
{
  char path[FAMILY_RESOURCE_PATH_BYTES];
  int player_index;
  int variant;
  int index;
  ResourceCatalogEntry *entry;
  MemoryHandle *node;
  char *variant_name;

  strcpy(path, (char *)(int)&(resource_root_path));

  player_index = race_mode_state.active_player_index;
  variant = player_profiles[player_index].current_variant;
  variant_name = course_variant_names[variant];

  strcat(path, variant_name);
  strcat(path, "/");
  strcat(path, variant_name);
  strcat(path, (char *)(int)&(course_resource_suffix));

  resource_catalog_handles.course_resource =
    RSRC_LoadFile(path, PREPARED_COURSE_RESOURCE_FILE_OPTIONS);
  if(resource_catalog_handles.course_resource == 0)
    {
      return 0;
    }

  index = 0;
  entry = course_catalog;
  while(entry->destination != 0)
    {
      if(entry->resource == 0)
        {
          node = RSRC_LoadResource(
            entry->chunk_type, entry->resource_index + 1,
            RESOURCE_LOAD_OPTION_KEEP_BUSY);
          entry->node = node;
          if(node == 0 || node->data == 0)
            {
              unload_course_resources();
              return 0;
            }
          node->flags |= MEM_HANDLE_FLAG_BUSY;
          entry->resource = node->data;
        }

      index++;
      entry++;
    }

  index = 0;
  entry = course_catalog;
  while(entry->destination != 0)
    {
      if(entry->resource != 0)
        {
          if(entry->chunk_type == CANS_RESOURCE_TYPE_CEL)
            {
              entry->resource = parse_cans_cel_chunk_stream(
                (CansChunkHeader *)entry->resource, 0);
              if(entry->resource == 0)
                {
                  unload_course_resources();
                  return 0;
                }
            }

          *(void **)entry->destination = entry->resource;
        }

      index++;
      entry++;
    }

  return finalize_loaded_segment_topology();
}
