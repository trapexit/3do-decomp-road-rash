#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

/*
 * The oracle uses only the read and write cursors, so a completely full ring
 * aliases an empty one.  Keep an out-of-layout count in the reconstruction so
 * unread requests cannot be overwritten.
 */
int family_resource_request_count;

int
enqueue_family_resource_request(int family_id,
                                int group_index,
                                int mode)
{
  FamilyResourceRequest *entry;
  int index;

  if(family_resource_globals.resident_family_ids[group_index] ==
     family_id)
    {
      return 1;
    }

  if(family_resource_request_count >= FAMILY_REQUEST_COUNT)
    {
      return 0;
    }

  index = family_resource_globals.request_write_index;
  entry = &family_resource_globals.request_queue[index];
  entry->group_index = group_index;
  entry->load_mode = mode;
  entry->family_id = family_id;

  index++;
  family_resource_globals.request_write_index = index;
  if(index >= FAMILY_REQUEST_COUNT)
    {
      family_resource_globals.request_write_index = 0;
    }
  family_resource_request_count++;
  return 1;
}
