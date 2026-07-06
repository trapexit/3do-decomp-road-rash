#include "rw_semantic_data.h"

int
RSRC_DisposeFileResources(void *resource_file);

void
unload_bike_specs_resource(void)
{
  int bike_index;

  for(bike_index = 0; bike_index < BIKE_SPEC_CACHE_COUNT; bike_index++)
    {
      race_rider_state.bike_spec_cache[bike_index] = 0;
    }
  if(race_rider_state.bike_specs_resource != 0)
    {
      RSRC_DisposeFileResources(
        race_rider_state.bike_specs_resource);
    }
  race_rider_state.bike_specs_resource = 0;
}
