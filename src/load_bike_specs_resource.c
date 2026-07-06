#include "resource_manager_runtime.h"
#include "rw_semantic_data.h"

#include "string.h"

enum BikeSpecsResourceConstant
{
  BIKE_SPECS_RESOURCE_PATH_CAPACITY = 0x34,
  BIKE_SPECS_FILE_LOAD_OPTIONS = 0x1000
};

static const char bike_specs_resource_name[] = "BikeSpecs.rsrc";

void
load_bike_specs_resource(void)
{
  char resource_path[BIKE_SPECS_RESOURCE_PATH_CAPACITY];
  MemoryHandle *resource_file;
  int bike_index;

  strcpy(resource_path, resource_root_path);
  strcat(resource_path, bike_specs_resource_name);

  resource_file =
    RSRC_OpenFile(resource_path, BIKE_SPECS_FILE_LOAD_OPTIONS);
  race_rider_state.bike_specs_resource = resource_file;
  if(resource_file == 0)
    {
      return;
    }

  for(bike_index = 0;
      bike_index < BIKE_SPEC_CACHE_COUNT;
      bike_index++)
    {
      race_rider_state.bike_spec_cache[bike_index] = 0;
    }
}
