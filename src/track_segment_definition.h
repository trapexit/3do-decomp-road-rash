#ifndef ROADRASH_TRACK_SEGMENT_DEFINITION_H
#define ROADRASH_TRACK_SEGMENT_DEFINITION_H

#include "stddef.h"

#include "track_traversal_runtime.h"

/* Exact layout of one entry in an RSGS resource. */
typedef struct TrackSegmentDefinition
{
  int sample_count;
  RoadPathResource *path_resource;
  RoadObjectResource *object_resource;
  RoadFamilyScheduleResource *family_schedule_resource;
  void *terrain_chunk;
  const RoadRsldResource *slope_geometry_resource;
  const RoadRlanResource *lane_width_resource;
  TrackHazardTable *hazard_table;
  RoadTrackGraphNode *parent_junction;
  RoadSectionResource *section_resource;
  int finish_distance;
  int finish_sample_offset;
  int segment_index;
} TrackSegmentDefinition;

enum RoadTrackGraphNodeType
{
  ROAD_TRACK_GRAPH_NODE_SEGMENT = 0,
  ROAD_TRACK_GRAPH_NODE_FORWARD_BRANCH = 1,
  ROAD_TRACK_GRAPH_NODE_REVERSE_BRANCH = 2
};

enum RoadTrackGraphNodeFlag
{
  ROAD_TRACK_GRAPH_NODE_VISITED = 1,
  ROAD_TRACK_GRAPH_NODE_RUNTIME_RELEASED = 0x10,
  ROAD_TRACK_GRAPH_NODE_FINISH_DISTANCE_SET = 0x20,
  ROAD_TRACK_GRAPH_NODE_PRIMARY_BRANCH = 0x40,
  ROAD_TRACK_GRAPH_NODE_SECONDARY_BRANCH = 0x80,
  ROAD_TRACK_GRAPH_NODE_BRANCH_MASK = 0xC0
};

typedef union RoadTrackGraphPayload
{
  TrackSegmentDefinition *segment_definition;
  RoadTrackGraphNode *single_link;
} RoadTrackGraphPayload;

typedef union RoadTrackGraphExtent
{
  int segment_sample_count;
  int transition_span;
} RoadTrackGraphExtent;

struct RoadTrackGraphNode
{
  int type;
  unsigned int flags;
  RoadTrackGraphNode *primary_link;
  RoadTrackGraphNode *secondary_link;
  RoadTrackGraphPayload payload;
  RoadTrackGraphExtent extent;
  signed char main_fork;
  signed char alternate_fork;
  unsigned char alignment[2];
  RoadTrackTransitionRuntime *transition_runtime;
};

int
create_road_track_transition_runtime(RoadTrackGraphNode *junction,
                                     int                 sample_direction);
int
fix_road_junction_elevations(RoadTrackGraphNode *junction);
int
prepare_track_resource_graph(RoadTrackGraphNode *node);
RoadTrackTransitionRuntime **
release_road_track_transition_runtime(RoadTrackGraphNode *node);
int
release_road_track_graph_transitions(RoadTrackGraphNode *root);

#define TRACK_SEGMENT_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntryStartOffsetIs0,
                            offsetof(RoadSectionEntry, start_sample) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntryEndOffsetIs4,
                            offsetof(RoadSectionEntry, end_sample) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntryKindOffsetIs8,
                            offsetof(RoadSectionEntry, kind) == 0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntrySelectorOffsetIs9,
                            offsetof(RoadSectionEntry, selector) == 0x09);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntryInsetOffsetIs10,
                            offsetof(RoadSectionEntry, collision_inset) ==
                            0x0A);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntryFlagsOffsetIs11,
                            offsetof(RoadSectionEntry,
                                     edge_collision_flags) == 0x0B);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionEntrySizeIs12,
                            sizeof(RoadSectionEntry) == 0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceTagOffsetIs0,
                            offsetof(RoadSectionResource, tag) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceSizeOffsetIs4,
                            offsetof(RoadSectionResource, byte_size) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceCountOffsetIs8,
                            offsetof(RoadSectionResource, entry_count) ==
                            0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceSamplesOffsetIs12,
                            offsetof(RoadSectionResource, sample_count) ==
                            0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceReservedOffsetIs16,
                            offsetof(RoadSectionResource, reserved_10) ==
                            0x10);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceEntriesOffsetIs20,
                            offsetof(RoadSectionResource, entries) == 0x14);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadSectionResourceMinimumSizeIs32,
                            sizeof(RoadSectionResource) == 0x20);

TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardEntryControlOffsetIs0,
                            offsetof(TrackHazardEntry, control_word) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardEntryDetailOffsetIs4,
                            offsetof(TrackHazardEntry, detail_word) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardEntrySizeIs8,
                            sizeof(TrackHazardEntry) == 0x08);

TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTableTagOffsetIs0,
                            offsetof(TrackHazardTable, tag) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTableSizeOffsetIs4,
                            offsetof(TrackHazardTable, byte_size) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTableCountOffsetIs8,
                            offsetof(TrackHazardTable, entry_count) == 0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTableReservedOffsetIs12,
                            offsetof(TrackHazardTable, reserved_0c) == 0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTableEntriesOffsetIs16,
                            offsetof(TrackHazardTable, entries) == 0x10);

TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTraversalTableOffsetIs0,
                            offsetof(TrackHazardTraversal, table) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTraversalPositionOffsetIs4,
                            offsetof(TrackHazardTraversal, track_position) ==
                            0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTraversalIndexOffsetIs8,
                            offsetof(TrackHazardTraversal, entry_index) ==
                            0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTraversalReserveOffsetIs12,
                            offsetof(TrackHazardTraversal,
                                     allocator_reserve_words) ==
                            0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackHazardTraversalSizeIs40,
                            sizeof(TrackHazardTraversal) == 0x28);

TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentSampleCountOffsetIs0,
                            offsetof(TrackSegmentDefinition, sample_count) ==
                            0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentPathOffsetIs4,
                            offsetof(TrackSegmentDefinition, path_resource) ==
                            0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentObjectsOffsetIs8,
                            offsetof(TrackSegmentDefinition,
                                     object_resource) == 0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentFamilyScheduleOffsetIs12,
                            offsetof(TrackSegmentDefinition,
                                     family_schedule_resource) == 0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentTerrainOffsetIs16,
                            offsetof(TrackSegmentDefinition, terrain_chunk) ==
                            0x10);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentSlopeOffsetIs20,
                            offsetof(TrackSegmentDefinition,
                                     slope_geometry_resource) == 0x14);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentLanesOffsetIs24,
                            offsetof(TrackSegmentDefinition,
                                     lane_width_resource) == 0x18);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentHazardsOffsetIs28,
                            offsetof(TrackSegmentDefinition, hazard_table) ==
                            0x1C);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentParentJunctionOffsetIs32,
                            offsetof(TrackSegmentDefinition,
                                     parent_junction) == 0x20);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentSectionsOffsetIs36,
                            offsetof(TrackSegmentDefinition,
                                     section_resource) ==
                            0x24);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentFinishDistanceOffsetIs40,
                            offsetof(TrackSegmentDefinition,
                                     finish_distance) == 0x28);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentFinishSampleOffsetIs44,
                            offsetof(TrackSegmentDefinition,
                                     finish_sample_offset) == 0x2C);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentIndexOffsetIs48,
                            offsetof(TrackSegmentDefinition, segment_index) ==
                            0x30);
TRACK_SEGMENT_LAYOUT_ASSERT(TrackSegmentDefinitionSizeIs52,
                            sizeof(TrackSegmentDefinition) == 0x34);

TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphPayloadSizeIs4,
                            sizeof(RoadTrackGraphPayload) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphExtentSizeIs4,
                            sizeof(RoadTrackGraphExtent) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeTypeOffsetIs0,
                            offsetof(RoadTrackGraphNode, type) == 0x00);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeFlagsOffsetIs4,
                            offsetof(RoadTrackGraphNode, flags) == 0x04);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodePrimaryOffsetIs8,
                            offsetof(RoadTrackGraphNode, primary_link) ==
                            0x08);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeSecondaryOffsetIs12,
                            offsetof(RoadTrackGraphNode, secondary_link) ==
                            0x0C);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodePayloadOffsetIs16,
                            offsetof(RoadTrackGraphNode, payload) == 0x10);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeExtentOffsetIs20,
                            offsetof(RoadTrackGraphNode, extent) == 0x14);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeMainForkOffsetIs24,
                            offsetof(RoadTrackGraphNode, main_fork) == 0x18);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeAlternateForkOffsetIs25,
                            offsetof(RoadTrackGraphNode, alternate_fork) ==
                            0x19);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeRuntimeOffsetIs28,
                            offsetof(RoadTrackGraphNode,
                                     transition_runtime) == 0x1C);
TRACK_SEGMENT_LAYOUT_ASSERT(RoadTrackGraphNodeSizeIs32,
                            sizeof(RoadTrackGraphNode) == 0x20);

#undef TRACK_SEGMENT_LAYOUT_ASSERT

#endif
