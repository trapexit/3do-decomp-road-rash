#include "memory_manager_runtime.h"
#include "segment_topology_runtime.h"
#include "string.h"

SegmentResourceRecord *
clone_segment_resource_record(const SegmentResourceRecord *source)
{
  SegmentResourceRecord *copy;
  SegmentResourceBlockHeader *nested;
  SegmentResourceBlockHeader **slot;
  int index;

  copy = MEM_NewPointer(SEGMENT_RESOURCE_RECORD_BYTES, MEMORY_REQUEST_ANY,
                        MEM_POINTER_OPTIONS_DEFAULT);
  if(copy == 0)
    {
      return 0;
    }

  memcpy(copy, source, sizeof(*copy));

  index = 0;
  do
    {
      slot = (SegmentResourceBlockHeader **)
             ((char *)copy + sizeof(int) + index * sizeof(void *));
      nested = *slot;
      if(nested != 0 &&
         (index == SEGMENT_RESOURCE_DIRECTION_SLOT ||
          index == SEGMENT_RESOURCE_WIDTH_SLOT ||
          index == SEGMENT_RESOURCE_BOUNDARY_SLOT ||
          index == SEGMENT_RESOURCE_CURVATURE_SLOT))
        {
          *slot = MEM_NewPointer(nested->byte_size, MEMORY_REQUEST_ANY,
                                 MEM_POINTER_OPTIONS_DEFAULT);
          if(*slot == 0)
            {
              return 0;
            }
          memcpy(*slot, nested, nested->byte_size);
        }
      index++;
    } while(index < SEGMENT_RESOURCE_POINTER_COUNT);

  return copy;
}
