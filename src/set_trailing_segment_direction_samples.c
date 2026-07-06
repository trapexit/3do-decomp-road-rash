#include "segment_topology_runtime.h"

int
set_trailing_segment_direction_samples(SegmentTopologyNode *node,
                                       int                  sample_count,
                                       int                  direction)
{
  SegmentDirectionTable *table;
  unsigned char *sample;
  int index;
  int limit;
  int value;

  table = node->payload.record->direction_table;
  limit = table->header.entry_count;
  index = limit - sample_count;
  if(index < 0)
    {
      index = 0;
    }

  value = (signed char)direction;
  while(index < limit)
    {
      sample = &table->sample_bytes[
        index * SEGMENT_DIRECTION_SAMPLE_BYTES];
      sample[SEGMENT_DIRECTION_VALUE_BYTE] = (unsigned char)value;
      sample[SEGMENT_DIRECTION_FLAGS_BYTE] = 0;
      index++;
      limit = table->header.entry_count;
    }

  return index;
}
