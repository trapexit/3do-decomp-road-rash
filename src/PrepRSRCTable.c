/* PrepRSRCTable - reconstructed from the 0x00039A28 routine. */

#include "resource_manager_runtime.h"

MemoryHandle *
RSRC_LoadResource(uint32 resource_type,
                  int32  resource_id,
                  uint32 options);

void
PrepRSRCTable(PreparedResourceTableHeader *table)
{
  int i;
  ResourceRecord *resource;

  table->reserved_0C = 0;
  for(i = 0; i < table->resource_count; i++)
    {
      resource = &table->records[i];
      if((resource->flags & RESOURCE_FLAG_PRELOAD) != 0)
        {
          resource->resource_handle = RSRC_LoadResource(
            resource->resource_type, resource->resource_id, 0);
        }
    }
}
