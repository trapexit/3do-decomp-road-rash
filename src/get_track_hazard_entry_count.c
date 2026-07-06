#include "track_segment_definition.h"

int
get_track_hazard_entry_count(const TrackHazardTable *table)
{
  unsigned int available_bytes;
  unsigned int available_entries;
  int declared_entries;

  if(table == 0 || table->tag != TRACK_HAZARD_RESOURCE_TAG ||
     table->byte_size < TRACK_HAZARD_TABLE_HEADER_BYTES)
    {
      return 0;
    }

  declared_entries = table->entry_count;
  if(declared_entries <= 0)
    {
      return 0;
    }

  available_bytes =
    table->byte_size - TRACK_HAZARD_TABLE_HEADER_BYTES;
  available_entries = available_bytes / sizeof(TrackHazardEntry);
  if((unsigned int)declared_entries > available_entries)
    {
      return (int)available_entries;
    }
  return declared_entries;
}
