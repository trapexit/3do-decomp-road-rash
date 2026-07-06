#include "memory_manager_runtime.h"
#include "segment_topology_runtime.h"
#include "stdlib.h"

SegmentResourceCollection *
relocate_segment_resource_collection(SegmentResourceCollection *resources)
{
  int index;
  int pointer_index;
  SegmentResourceRecord *record;
  void **slot;
  int value;

  record = resources->inline_records;
  resources->records = record;
  if(resources->record_count <= 0)
    {
      return resources;
    }

  index = 0;
  do
    {
      pointer_index = 0;
      do
        {
          slot = (void **)((char *)record + sizeof(int) +
                           pointer_index * sizeof(void *));
          if(*slot != 0)
            {
              *slot = (char *)resources + (int)*slot;
            }
          pointer_index++;
        } while(pointer_index < SEGMENT_RESOURCE_POINTER_COUNT);

      value = index + 1;
      record->segment_index = value;
      record->endpoint_index =
        record->direction_table->header.entry_count;
      record++;
      index = value;
    } while(index < resources->record_count);

  return resources;
}


SegmentWorkBuffer *
allocate_segment_work_buffer(int payload_bytes)
{
  SegmentWorkBuffer *buffer;

  buffer = MEM_NewPointer(payload_bytes + sizeof(SegmentWorkBuffer),
                          MEMORY_REQUEST_ANY,
                          MEM_POINTER_OPTIONS_DEFAULT);
  if(buffer != 0)
    {
      buffer->payload_bytes = payload_bytes;
      buffer->payload = (unsigned char *)(buffer + 1);
    }
  return buffer;
}


char *
choose_random_byte_address(char *first,
                           char *last)
{
  int span;
  unsigned int mask;
  unsigned int size;
  unsigned int index;

  span = (last + 1) - first;
  size = 1;
  if(span > 1)
    {
      do
        {
          size <<= 1;
        } while(size < (unsigned int)span);
    }

  mask = size - 1;
  do
    {
      index = (unsigned int)rand() & mask;
    } while(index >= (unsigned int)span);

  return first + index;
}
