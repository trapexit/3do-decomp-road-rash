#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"

static
void
repair_primary_segment_topology(SegmentTopologyNode *node);

int
read_segment_boundary_modes(const SegmentTopologyNode *node,
                            int                        read_front,
                            unsigned int              *primary_mode,
                            unsigned int              *secondary_mode)
{
  SegmentBoundaryTable *table;
  SegmentBoundaryEntry *entry;
  int index;

  read_front = (unsigned char)read_front;
  table = node->payload.record->boundary_table;
  if(table == 0)
    {
      return 0;
    }

  if(read_front != 0)
    {
      entry = table->entries;
      *primary_mode = entry->front_primary_mode;
      *secondary_mode = entry->front_secondary_mode;
    }
  else
    {
      index = table->header.entry_count - 1;
      entry = &table->entries[index];
      *primary_mode = entry->back_primary_mode;
      *secondary_mode = entry->back_secondary_mode;
    }

  return 1;
}


void
reconcile_segment_junction_boundaries(SegmentTopologyNode *node)
{
  SegmentTopologyNode *source;
  SegmentTopologyNode *left;
  SegmentTopologyNode *right;
  unsigned int source_a;
  unsigned int source_b;
  unsigned int left_a;
  unsigned int left_b;
  unsigned int right_a;
  unsigned int right_b;
  int front;

  source = node->payload.source;
  left = node->branches[0];
  right = node->branches[1];

  front = node->kind == SEGMENT_TOPOLOGY_PRIMARY_JUNCTION;
  if(read_segment_boundary_modes(
       source, front, &source_a, &source_b) == 0)
    {
      return;
    }
  if(read_segment_boundary_modes(
       left, !front, &left_a, &left_b) == 0)
    {
      return;
    }
  if(read_segment_boundary_modes(
       right, !front, &right_a, &right_b) == 0)
    {
      return;
    }

  if(right_b != 0 && left_a != 0)
    {
      return;
    }

  set_segment_boundary_modes(left, right_a, right_b);
  set_segment_boundary_modes(right, left_a, left_b);
}


static
void
repair_primary_segment_topology(SegmentTopologyNode *node)
{
  SegmentResourceRecord *copy;
  SegmentTopologyNode *new_leaf;
  SegmentTopologyNode *old_branch;
  SegmentTopologyNode *split;
  int index;
  SegmentTopologyKind kind;

  kind = node->branches[0]->kind;
  switch(kind)
    {
    case SEGMENT_TOPOLOGY_LEAF:
      split = allocate_segment_topology_junction(
        SEGMENT_TOPOLOGY_PRIMARY_JUNCTION);
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

      new_leaf->branches[0] = (SegmentTopologyNode *)
                              &segment_repair_globals.repair_pass_count;
      old_branch = node->branches[0];
      node->branches[0] = split;
      old_branch->branches[1] = split;
      new_leaf->branches[1] = split;
      split->branches[0] = new_leaf;
      split->branches[1] = old_branch;
      split->payload.source = node;

      set_all_segment_direction_samples(old_branch, 1, 1);
      set_all_segment_direction_samples(new_leaf, 1, -1);
      set_segment_boundary_modes(
        new_leaf, SEGMENT_BOUNDARY_CONTINUES,
        SEGMENT_BOUNDARY_UNUSED);
      set_segment_boundary_modes(
        old_branch, SEGMENT_BOUNDARY_CONTINUES,
        SEGMENT_BOUNDARY_TERMINATES);
      set_segment_boundary_modes(
        node, SEGMENT_BOUNDARY_TERMINATES,
        SEGMENT_BOUNDARY_TERMINATES);
      return;

    case SEGMENT_TOPOLOGY_PRIMARY_JUNCTION:
      return;

    case SEGMENT_TOPOLOGY_SECONDARY_JUNCTION:
      index = node->selected_branch;
      repair_primary_segment_topology(node->branches[index]);
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
      repair_primary_segment_topology(node);
      return;
    }
}
