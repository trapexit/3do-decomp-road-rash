/* Reconstructed from the original function at 0x0001A358. */



#include "family_resource_runtime.h"
#include "rw_semantic_data.h"

int
RSRC_SeekResource(int,
                  int);

int
seek_family_resource(const FamilyResourceWorkerRequest *request)
{
  int result;

  result = RSRC_SeekResource(FAMILY_RESOURCE_TYPE_TAG,
                             request->family_id);
  if(result != 0)
    {
      family_resource_failure_codes[request->group_index] =
        FAMILY_RESOURCE_LOAD_FAILURE_SEEK;
      family_resource_failure_values[request->group_index] = result;
    }
  family_resource_globals.last_seek_result = result;
  return result;
}
