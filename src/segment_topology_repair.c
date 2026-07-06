#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"

void
set_segment_boundary_modes(SegmentTopologyNode *node,
                           int                  primary_mode,
                           int                  secondary_mode)
{
  SegmentBoundaryTable *table;
  SegmentBoundaryEntry *entry;
  int index;
  int primary;
  int secondary;

  table = node->payload.record->boundary_table;
  if(table == 0)
    {
      return;
    }

  if(table->header.entry_count == 0)
    {
      return;
    }

  entry = table->entries;
  index = 0;
  primary = (unsigned char)primary_mode;
  secondary = (unsigned char)secondary_mode;
  do
    {
      entry->front_primary_mode = (unsigned char)primary;
      entry->front_secondary_mode = (unsigned char)secondary;
      entry->back_primary_mode = (unsigned char)primary;
      entry->back_secondary_mode = (unsigned char)secondary;
      entry->primary_link = 0;
      entry->secondary_link = 0;
      index++;
      entry++;
    } while((unsigned int)index <
            (unsigned int)table->header.entry_count);
}


void
repair_secondary_segment_topology(SegmentTopologyNode *node)
{
  SegmentResourceRecord *copy;
  SegmentTopologyNode *new_leaf;
  SegmentTopologyNode *split;
  SegmentTopologyNode *old_branch;
  SegmentTopologyKind kind;
  int index;

  kind = node->branches[0]->kind;
  switch(kind)
    {
    case SEGMENT_TOPOLOGY_LEAF:
      split = allocate_segment_topology_junction(
        SEGMENT_TOPOLOGY_SECONDARY_JUNCTION);
      if(split == 0)
        {
          return;
        }

      copy = clone_segment_resource_record(node->payload.record);
      if(copy == 0)
        {
          return;
        }

      new_leaf = create_segment_topology_leaf(copy);
      if(new_leaf == 0)
        {
          return;
        }

      new_leaf->branches[1] = (SegmentTopologyNode *)
                              &segment_repair_globals.destination_count;
      old_branch = node->branches[0];
      set_trailing_segment_direction_samples(
        node, SEGMENT_DIRECTION_REPAIR_WINDOW, 0);
      set_trailing_segment_direction_samples(
        new_leaf, SEGMENT_DIRECTION_REPAIR_WINDOW, 0);

      node->branches[0] = split;
      old_branch->branches[1] = split;
      new_leaf->branches[0] = split;
      set_trailing_segment_direction_samples(node, 1, -1);
      set_trailing_segment_direction_samples(new_leaf, 1, 1);

      split->branches[0] = node;
      split->branches[1] = new_leaf;
      split->payload.source = old_branch;

      set_segment_boundary_modes(
        node, SEGMENT_BOUNDARY_TERMINATES,
        SEGMENT_BOUNDARY_CONTINUES);
      set_segment_boundary_modes(
        new_leaf, SEGMENT_BOUNDARY_UNUSED,
        SEGMENT_BOUNDARY_CONTINUES);
      set_segment_boundary_modes(
        old_branch, SEGMENT_BOUNDARY_TERMINATES,
        SEGMENT_BOUNDARY_TERMINATES);
      return;

    case SEGMENT_TOPOLOGY_PRIMARY_JUNCTION:
      index = node->selected_branch;
      repair_secondary_segment_topology(node->branches[index]);
      return;

    case SEGMENT_TOPOLOGY_SECONDARY_JUNCTION:
      return;

    case SEGMENT_TOPOLOGY_FORWARD_REFERENCE:
    case SEGMENT_TOPOLOGY_REVERSE_REFERENCE:
      copy = clone_segment_resource_record(node->payload.record);
      if(copy == 0)
        {
          return;
        }

      new_leaf = create_segment_topology_leaf(copy);
      if(new_leaf == 0)
        {
          return;
        }

      new_leaf->branches[0] = node->branches[0];
      new_leaf->branches[1] = node;
      node->branches[0] = new_leaf;
      repair_secondary_segment_topology(node);
      return;
    }
}
