#include "memory_manager_runtime.h"
#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"
#include "string.h"

void
normalize_segment_leaf_resource(SegmentTopologyNode *node,
                                int                  unused_record_address)
{
  SegmentResourceRecord *record;
  SegmentBoundaryTable *boundaries;
  SegmentBoundaryEntry *boundary;
  SegmentWidthTable *widths;
  SegmentWidthTable *new_widths;
  SegmentWidthEntry *width;
  SegmentHillTable *hills;
  int *hill_cursor;
  SegmentCompactTable *compact;
  SegmentCompactEntry *compact_entry;
  int target;
  int count;
  int index;
  int value;

  (void)unused_record_address;

  record = node->payload.record;
  target = node->endpoint_index;
  compact = record->compact_table;

  boundaries = record->boundary_table;
  boundary = &boundaries->entries[boundaries->header.entry_count - 1];
  if(boundary->end_position != target)
    {
      boundary->end_position = target;
      boundary->start_position = target;
    }

  widths = record->width_table;
  index = widths->header.entry_count - 1;
  if(index == 0)
    {
      new_widths = MEM_NewPointer(
        sizeof(SegmentResourceBlockHeader) +
        sizeof(SegmentWidthEntry) *
        SEGMENT_DUPLICATED_WIDTH_ENTRY_COUNT,
        MEMORY_REQUEST_ANY, MEM_POINTER_OPTIONS_DEFAULT);
      if(new_widths != 0)
        {
          memcpy(new_widths, widths,
                 sizeof(SegmentResourceBlockHeader) +
                 sizeof(SegmentWidthEntry));
          new_widths->header.entry_count =
            SEGMENT_DUPLICATED_WIDTH_ENTRY_COUNT;
          new_widths->entries[1] = new_widths->entries[0];
          new_widths->entries[1].start_endpoint = target;
          new_widths->entries[1].end_endpoint = target;
          record->width_table = new_widths;
        }
    }
  else
    {
      width = &widths->entries[index];
      if(width->end_endpoint != target)
        {
          width->end_endpoint = target;
          width->start_endpoint = target;
        }
      if(width->back_primary_mode != width->front_primary_mode ||
         width->back_secondary_mode != width->front_secondary_mode)
        {
          width->back_primary_mode = width->front_primary_mode;
          width->back_secondary_mode = width->front_secondary_mode;
        }
    }

  hills = record->hill_table;
  if(hills->header.resource_tag == SEGMENT_HILL_RESOURCE_TAG)
    {
      count = hills->header.entry_count - 1;
      if(count != 0)
        {
          hill_cursor = (int *)hills +
                        count * SEGMENT_HILL_WORDS_PER_ENTRY;
          if(hill_cursor[SEGMENT_HILL_PREVIOUS_ENDPOINT_WORD] == target ||
             hill_cursor[SEGMENT_HILL_NEXT_SPAN_WORD] == 0)
            {
              hills->header.entry_count = count;
            }
        }
    }

  if(compact == 0)
    {
      return;
    }

  if(compact->endpoint_index != target)
    {
      compact->endpoint_index = target;
    }

  compact_entry = compact->entries;
  index = 0;
  while((unsigned int)index < (unsigned int)compact->entry_count)
    {
      value = compact_entry->end_endpoint -
              compact_entry->start_endpoint - SEGMENT_COMPACT_SPAN_MARGIN;
      if(value < 1)
        {
          value = compact_entry->direction +
                  SEGMENT_COMPACT_DIRECTION_BIAS;
          if(value >= 0 && value <= SEGMENT_COMPACT_DIRECTION_LIMIT)
            {
              count = index;
              while(count < compact->entry_count - 1)
                {
                  compact->entries[count] = compact->entries[count + 1];
                  count++;
                }
              compact->entry_count--;
            }
        }
      index++;
      compact_entry++;
    }
}
