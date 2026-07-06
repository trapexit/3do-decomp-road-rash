#include "string.h"

#include "filestreamfunctions.h"

#include "division_helpers.h"
#include "half_one_over_resource.h"
#include "memory_manager_runtime.h"
#include "road_renderer_runtime.h"
#include "rw_semantic_data.h"

enum RoadReciprocalTableConstant
{
  ROAD_RECIPROCAL_STREAM_BUFFER_BYTES = 0x1000,
  ROAD_RECIPROCAL_DEFAULT_PHASE_STEP = 0x10000,
  ROAD_RECIPROCAL_SAMPLE_SHIFT = 10,
  ROAD_RECIPROCAL_PHASE_STEP_SHIFT = 1,
  ROAD_RECIPROCAL_MINIMUM_INDEX = 0x69
};

#define ROAD_RECIPROCAL_FIXED_NUMERATOR 0x40000000U

int
load_road_reciprocal_table(void)
{
  char path[HALF_ONE_OVER_TABLE_PATH_SIZE];
  Stream *stream;
  int result;

  memcpy(path, half_one_over_table_path, sizeof(path));
  stream = OpenDiskStream(path, ROAD_RECIPROCAL_STREAM_BUFFER_BYTES);
  if(stream == 0)
    {
      return 0;
    }

  result = ReadDiskStream(
    stream, (char *)road_renderer_state.reciprocal_table,
    half_one_over_table_entry_count * sizeof(int));
  if(result != half_one_over_table_entry_count * sizeof(int))
    {
      CloseDiskStream(stream);
      return 0;
    }

  CloseDiskStream(stream);
  return 1;
}


int
initialize_road_reciprocal_table(void)
{
  int result;

  if(road_renderer_state.reciprocal_table == 0)
    {
      road_renderer_state.reciprocal_table = (int *)MEM_NewPointer(
        half_one_over_table_entry_count * sizeof(int),
        MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
    }
  if(road_renderer_state.reciprocal_table == 0)
    {
      return 0;
    }

  result = load_road_reciprocal_table();
  if(result == 0)
    {
      return generate_road_reciprocal_table(
        ROAD_RECIPROCAL_DEFAULT_PHASE_STEP);
    }
  return result;
}


int
generate_road_reciprocal_table(int phase_step)
{
  SignedDivisionResult division;
  int *table;
  int count;
  int index;
  int phase;
  int sample;
  int fill;

  if(road_renderer_state.reciprocal_table == 0)
    {
      road_renderer_state.reciprocal_table = (int *)MEM_NewPointer(
        half_one_over_table_entry_count * sizeof(int),
        MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
    }
  table = road_renderer_state.reciprocal_table;
  if(table == 0)
    {
      return 0;
    }

  count = half_one_over_table_entry_count;
  phase = 0;
  index = 0;
  if(count > 0)
    {
      do
        {
          sample = phase >> ROAD_RECIPROCAL_SAMPLE_SHIFT;
          if(sample == 0)
            {
              table[index] = 0;
            }
          else
            {
              division = signed_divide_with_remainder(
                (unsigned int)sample,
                ROAD_RECIPROCAL_FIXED_NUMERATOR);
              table[index] = division.quotient;
            }
          index++;
          phase += phase_step << ROAD_RECIPROCAL_PHASE_STEP_SHIFT;
        } while(index < count);
    }

  fill = table[ROAD_RECIPROCAL_MINIMUM_INDEX];
  index = 0;
  do
    {
      table[index] = fill;
      index++;
    } while(index < ROAD_RECIPROCAL_MINIMUM_INDEX);

  return 1;
}


#undef ROAD_RECIPROCAL_FIXED_NUMERATOR
