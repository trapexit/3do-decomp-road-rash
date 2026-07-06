#include "bss_late_data.h"

/* Reconstructed from the original function at 0x00021250. */

void
get_road_section_collision_attributes(const RoadSectionTraversalState *traversal,
                                      unsigned char                   *out_collision_kind,
                                      unsigned char                   *out_selector,
                                      signed char                     *out_collision_inset,
                                      unsigned char                   *out_edge_collision_flags)
{
  const RoadSectionEntry *entry;

  if(traversal->resource == 0)
    {
      *out_collision_kind = 0;
      *out_selector = 0;
      *out_collision_inset = 0;
      *out_edge_collision_flags = 0;
      return;
    }

  *out_collision_kind = traversal->active_kind;
  entry = traversal->active_entry;
  *out_selector = (unsigned char)entry->selector;
  *out_collision_inset = entry->collision_inset;
  *out_edge_collision_flags = entry->edge_collision_flags;
}
