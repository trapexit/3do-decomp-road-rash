#include "resource_manager_runtime.h"

/* Reconstructed from the 0x00039E8C entry of the original function at 0x00039E8C. */

ResourceRecord *
find_resource_record_in_table(ResourceTableBlock *table,
                              uint32              resource_type,
                              int32               resource_id)
{
  ResourceRecord *records;
  ResourceRecord *candidate;
  ResourceRecord *found;
  int lower_index;
  int upper_index;
  int middle_index;

  lower_index = 0;
  found = 0;
  upper_index = table->last_resource_index;
  records = (ResourceRecord *)((char *)table + sizeof(*table));

  while(lower_index <= upper_index && found == 0)
    {
      middle_index = (upper_index + lower_index) >> 1;
      candidate = &records[middle_index];

      if((int32)candidate->resource_type == (int32)resource_type &&
         candidate->resource_id == resource_id)
        {
          found = candidate;
        }
      else if((int32)candidate->resource_type >
              (int32)resource_type ||
              ((int32)candidate->resource_type ==
               (int32)resource_type &&
               candidate->resource_id > resource_id))
        {
          upper_index = middle_index - 1;
        }
      else
        {
          lower_index = middle_index + 1;
        }
    }

  return found;
}
