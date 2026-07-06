#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"

int
connect_secondary_segment_junction(SegmentTopologyNode *primary_leaf,
                                   SegmentTopologyNode *secondary_leaf,
                                   SegmentTopologyNode *reference)
{
  SegmentTopologyNode *junction;

  junction = allocate_segment_topology_junction(
    SEGMENT_TOPOLOGY_SECONDARY_JUNCTION);
  if(junction == 0)
    {
      return 0;
    }

  primary_leaf->branches[SEGMENT_BRANCH_PRIMARY] = junction;
  secondary_leaf->branches[SEGMENT_BRANCH_PRIMARY] = junction;
  reference->branches[SEGMENT_BRANCH_SECONDARY] = junction;

  junction->branches[SEGMENT_BRANCH_PRIMARY] = primary_leaf;
  junction->branches[SEGMENT_BRANCH_SECONDARY] = secondary_leaf;
  junction->payload.source = reference;

  set_trailing_segment_direction_samples(
    primary_leaf, SEGMENT_JOIN_DIRECTION_WINDOW,
    -SEGMENT_JOIN_DIRECTION_DELTA);
  return set_trailing_segment_direction_samples(
    secondary_leaf, SEGMENT_JOIN_DIRECTION_WINDOW,
    SEGMENT_JOIN_DIRECTION_DELTA);
}


void
repair_segment_topology_reference(SegmentTopologyNode *reference)
{
  SegmentTopologyContainer *container;
  SegmentTopologyNode *primary_endpoint;
  SegmentTopologyNode *secondary_endpoint;
  SegmentResourceRecord *record_copy;
  SegmentTopologyNode *primary_leaf;
  SegmentTopologyNode *secondary_leaf;

  container = (SegmentTopologyContainer *)
              segment_repair_globals.current_segment;
  primary_endpoint = find_segment_branch_reference_parent(
    container->root, SEGMENT_BRANCH_PRIMARY);
  secondary_endpoint = find_segment_branch_reference_parent(
    container->root, SEGMENT_BRANCH_SECONDARY);

  if(primary_endpoint != secondary_endpoint)
    {
      record_copy = clone_segment_resource_record(
        primary_endpoint->payload.record);
      if(record_copy == 0)
        {
          return;
        }

      primary_leaf = create_segment_topology_leaf(record_copy);
      if(primary_leaf == 0)
        {
          return;
        }

      record_copy = clone_segment_resource_record(
        secondary_endpoint->payload.record);
      if(record_copy == 0)
        {
          return;
        }

      secondary_leaf = create_segment_topology_leaf(record_copy);
      if(secondary_leaf == 0)
        {
          return;
        }

      primary_endpoint->branches[SEGMENT_BRANCH_PRIMARY] = primary_leaf;
      primary_leaf->branches[SEGMENT_BRANCH_SECONDARY] =
        primary_endpoint;
      secondary_endpoint->branches[SEGMENT_BRANCH_PRIMARY] =
        secondary_leaf;
      secondary_leaf->branches[SEGMENT_BRANCH_SECONDARY] =
        secondary_endpoint;
      connect_secondary_segment_junction(
        primary_leaf, secondary_leaf, reference);
    }
  else
    {
      primary_endpoint = find_segment_branch_reference_parent(
        reference, SEGMENT_BRANCH_SELECTED);
      reference->branches[SEGMENT_BRANCH_SECONDARY] = primary_endpoint;
      primary_endpoint->branches[SEGMENT_BRANCH_PRIMARY] = reference;
    }

  redirect_segment_topology_references(reference, reference);
}
