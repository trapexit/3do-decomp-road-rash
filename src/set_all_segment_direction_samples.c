#include "segment_topology_runtime.h"

void
set_all_segment_direction_samples(SegmentTopologyNode *node,
                                  int                  unused_count,
                                  int                  direction)
{
  SegmentDirectionTable *table;
  unsigned char *sample;
  int index;
  signed char value;

  (void)unused_count;
  table = node->payload.record->direction_table;
  index = 0;
  if(table->header.entry_count <= 0)
    {
      return;
    }

  value = (signed char)direction;
  do
    {
      sample = &table->sample_bytes[
        index * SEGMENT_DIRECTION_SAMPLE_BYTES];
      sample[SEGMENT_DIRECTION_VALUE_BYTE] = (unsigned char)value;
      sample[SEGMENT_DIRECTION_FLAGS_BYTE] = 0;
      index++;
    } while(index < table->header.entry_count);
}
