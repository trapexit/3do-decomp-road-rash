/* Reconstructed from the original function at 0x0001AB1C. */



#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

void
dispatch_pending_family_resource_request(void)
{
  int index;
  FamilyResourceRequest *entry;

  index = family_resource_globals.request_read_index;

  if(family_resource_request_count <= 0)
    {
      return;
    }

  entry = &family_resource_globals.request_queue[index];
  if(submit_family_resource_worker_request(entry) != 0)
    {
      return;
    }

  index++;
  family_resource_globals.request_read_index = index;
  if(index >= FAMILY_REQUEST_COUNT)
    {
      family_resource_globals.request_read_index = 0;
    }
  family_resource_request_count--;
}
