#include "intrusive_list.h"
#include "rw_semantic_data.h"

void
destroy_track_allocation_pools(void)
{
  IntrusiveListStorage *storage;

  storage = (IntrusiveListStorage *)
            track_runtime_globals.bucket_handles[0];
  if(storage != 0)
    {
      return_all_intrusive_list_nodes_to_owner(storage);
      dispose_intrusive_list_storage_if_empty(storage);
      track_runtime_globals.bucket_handles[0] = 0;
    }

  storage = (IntrusiveListStorage *)
            track_runtime_globals.bucket_handles[1];
  if(storage != 0)
    {
      return_all_intrusive_list_nodes_to_owner(storage);
      dispose_intrusive_list_storage_if_empty(storage);
      track_runtime_globals.bucket_handles[1] = 0;
    }

  storage = (IntrusiveListStorage *)
            track_runtime_globals.allocation_pool;
  if(storage != 0)
    {
      destroy_intrusive_object_pool(storage);
      track_runtime_globals.allocation_pool = 0;
    }
}
