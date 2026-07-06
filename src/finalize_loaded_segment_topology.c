#include "rw_semantic_data.h"
#include "segment_topology_runtime.h"

SegmentTopologyNode *
finalize_loaded_segment_topology(void)
{
  SegmentTopologyContainer *container;
  SegmentResourceCollection *resources;

  container = (SegmentTopologyContainer *)
              segment_repair_globals.current_segment;
  container->root = relocate_segment_topology_offsets(
    container, (int)container->root);

  resources = (SegmentResourceCollection *)
              segment_repair_globals.resource_destination;
  relocate_segment_resource_collection(resources);

  relocate_and_repair_segment_topology(
    container->root, resources);

  container = (SegmentTopologyContainer *)
              segment_repair_globals.current_segment;
  return container->root;
}
