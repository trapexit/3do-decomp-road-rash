#include "family_resource_runtime.h"
#include "resource_manager_runtime.h"

void *
load_family_resource_chunk_into_group(int   family_id,
                                      void *group_destination)
{
  return RSRC_StuffResource(
    FAMILY_RESOURCE_TYPE_TAG, family_id, group_destination);
}
