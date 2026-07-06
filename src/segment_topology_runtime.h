#ifndef SEGMENT_TOPOLOGY_RUNTIME_H
#define SEGMENT_TOPOLOGY_RUNTIME_H

#ifndef offsetof
  #include "stddef.h"
#endif

enum
{
  SEGMENT_RESOURCE_RECORD_BYTES = 0x34,
  SEGMENT_TOPOLOGY_NODE_BYTES = 0x20,
  SEGMENT_JOIN_DIRECTION_WINDOW = 0x32,
  SEGMENT_JOIN_DIRECTION_DELTA = 2,
  SEGMENT_DIRECTION_REPAIR_WINDOW = 0x6E,
  SEGMENT_TOPOLOGY_UNSET_ENDPOINT = 0x6400,
  SEGMENT_RESOURCE_POINTER_COUNT = 12,
  SEGMENT_DIRECTION_SAMPLE_BYTES = 2,
  SEGMENT_DIRECTION_VALUE_BYTE = 0,
  SEGMENT_DIRECTION_FLAGS_BYTE = 1,
  SEGMENT_HILL_WORDS_PER_ENTRY = 19,
  SEGMENT_HILL_PREVIOUS_ENDPOINT_WORD = -13,
  SEGMENT_HILL_NEXT_SPAN_WORD = 6,
  SEGMENT_COMPACT_SPAN_MARGIN = 2,
  SEGMENT_COMPACT_DIRECTION_BIAS = 1,
  SEGMENT_COMPACT_DIRECTION_LIMIT = 3,
  SEGMENT_DUPLICATED_WIDTH_ENTRY_COUNT = 2
};

#define SEGMENT_HILL_RESOURCE_TAG 0x5248494C

typedef enum SegmentTopologyKind
{
  SEGMENT_TOPOLOGY_LEAF = 0,
  SEGMENT_TOPOLOGY_PRIMARY_JUNCTION = 1,
  SEGMENT_TOPOLOGY_SECONDARY_JUNCTION = 2,
  SEGMENT_TOPOLOGY_FORWARD_REFERENCE = 3,
  SEGMENT_TOPOLOGY_REVERSE_REFERENCE = 4
} SegmentTopologyKind;

typedef enum SegmentResourcePointerSlot
{
  SEGMENT_RESOURCE_DIRECTION_SLOT = 0,
  SEGMENT_RESOURCE_WIDTH_SLOT = 4,
  SEGMENT_RESOURCE_BOUNDARY_SLOT = 5,
  SEGMENT_RESOURCE_CURVATURE_SLOT = 6
} SegmentResourcePointerSlot;

typedef enum SegmentBoundaryMode
{
  SEGMENT_BOUNDARY_UNUSED = 0,
  SEGMENT_BOUNDARY_CONTINUES = 1,
  SEGMENT_BOUNDARY_TERMINATES = 2
} SegmentBoundaryMode;

typedef enum SegmentTopologyTraversalFlag
{
  SEGMENT_TOPOLOGY_POINTERS_RELOCATED = 2,
  SEGMENT_TOPOLOGY_FINALIZED = 4,
  SEGMENT_TOPOLOGY_REFERENCE_VISITED = 8
} SegmentTopologyTraversalFlag;

typedef enum SegmentBranchSelector
{
  SEGMENT_BRANCH_PRIMARY = 0,
  SEGMENT_BRANCH_SECONDARY = 1,
  SEGMENT_BRANCH_SELECTED = 2,
  SEGMENT_BRANCH_ALTERNATE = 3
} SegmentBranchSelector;

typedef struct SegmentResourceBlockHeader
{
  int resource_tag;
  int byte_size;
  int entry_count;
  int resource_flags;
} SegmentResourceBlockHeader;

typedef struct SegmentDirectionTable
{
  SegmentResourceBlockHeader header;
  unsigned char sample_bytes[1];
} SegmentDirectionTable;

typedef struct SegmentBoundaryEntry
{
  int start_position;
  int end_position;
  unsigned char front_primary_mode;
  unsigned char front_secondary_mode;
  unsigned char back_primary_mode;
  unsigned char back_secondary_mode;
  int primary_link;
  int secondary_link;
} SegmentBoundaryEntry;

typedef struct SegmentBoundaryTable
{
  SegmentResourceBlockHeader header;
  SegmentBoundaryEntry entries[1];
} SegmentBoundaryTable;

typedef struct SegmentWidthEntry
{
  int start_endpoint;
  int end_endpoint;
  int width_value;
  unsigned char front_primary_mode;
  unsigned char front_secondary_mode;
  unsigned char back_primary_mode;
  unsigned char back_secondary_mode;
  int shape_values[5];
} SegmentWidthEntry;

typedef struct SegmentWidthTable
{
  SegmentResourceBlockHeader header;
  SegmentWidthEntry entries[1];
} SegmentWidthTable;

typedef struct SegmentHillTable
{
  SegmentResourceBlockHeader header;
  int entry_words[1];
} SegmentHillTable;

typedef struct SegmentCompactEntry
{
  int start_endpoint;
  int end_endpoint;
  signed char direction;
  unsigned char entry_flags[3];
} SegmentCompactEntry;

typedef struct SegmentCompactTable
{
  int resource_tag;
  int byte_size;
  int entry_count;
  int endpoint_index;
  int table_flags;
  SegmentCompactEntry entries[1];
} SegmentCompactTable;

typedef struct SegmentResourceRecord
{
  int endpoint_index;
  SegmentDirectionTable *direction_table;
  void *surface_table;
  struct RoadFamilyScheduleResource *family_schedule_resource;
  SegmentHillTable *hill_table;
  SegmentWidthTable *width_table;
  SegmentBoundaryTable *boundary_table;
  void *curvature_table;
  void *scenery_table;
  SegmentCompactTable *compact_table;
  void *collision_table;
  void *event_table;
  int segment_index;
} SegmentResourceRecord;

typedef struct SegmentResourceCollection
{
  int resource_tag;
  int byte_size;
  int record_count;
  SegmentResourceRecord *records;
  SegmentResourceRecord inline_records[1];
} SegmentResourceCollection;

typedef struct SegmentTopologyNode SegmentTopologyNode;

typedef struct SegmentTopologyContainer
{
  int resource_tag;
  int byte_size;
  int node_count;
  int container_flags;
  SegmentTopologyNode *root;
} SegmentTopologyContainer;

typedef struct SegmentWorkBuffer
{
  int payload_bytes;
  unsigned char *payload;
  int allocation_flags;
} SegmentWorkBuffer;

typedef union SegmentTopologyPayload
{
  SegmentResourceRecord *record;
  SegmentTopologyNode *source;
} SegmentTopologyPayload;

struct SegmentTopologyNode
{
  int kind;
  int traversal_flags;
  SegmentTopologyNode *branches[2];
  SegmentTopologyPayload payload;
  int endpoint_index;
  signed char selected_branch;
  unsigned char alternate_branch;
  unsigned char branch_alignment[2];
  int traversal_value;
};

typedef char SegmentResourceHeaderSizeCheck[
    sizeof(SegmentResourceBlockHeader) == 0x10 ? 1 : -1];
typedef char SegmentBoundaryEntrySizeCheck[
    sizeof(SegmentBoundaryEntry) == 0x14 ? 1 : -1];
typedef char SegmentWidthEntrySizeCheck[
    sizeof(SegmentWidthEntry) == 0x24 ? 1 : -1];
typedef char SegmentCompactEntrySizeCheck[
    sizeof(SegmentCompactEntry) == 0x0C ? 1 : -1];
typedef char SegmentCompactEntriesOffsetCheck[
    offsetof(SegmentCompactTable, entries) == 0x14 ? 1 : -1];
typedef char SegmentResourceRecordSizeCheck[
    sizeof(SegmentResourceRecord) == SEGMENT_RESOURCE_RECORD_BYTES ? 1 : -1];
typedef char SegmentResourceDirectionOffsetCheck[
    offsetof(SegmentResourceRecord, direction_table) == 0x04 ? 1 : -1];
typedef char SegmentResourceBoundaryOffsetCheck[
    offsetof(SegmentResourceRecord, boundary_table) == 0x18 ? 1 : -1];
typedef char SegmentResourceIndexOffsetCheck[
    offsetof(SegmentResourceRecord, segment_index) == 0x30 ? 1 : -1];
typedef char SegmentTopologyNodeSizeCheck[
    sizeof(SegmentTopologyNode) == SEGMENT_TOPOLOGY_NODE_BYTES ? 1 : -1];
typedef char SegmentTopologyBranchOffsetCheck[
    offsetof(SegmentTopologyNode, branches) == 0x08 ? 1 : -1];
typedef char SegmentTopologyPayloadOffsetCheck[
    offsetof(SegmentTopologyNode, payload) == 0x10 ? 1 : -1];
typedef char SegmentTopologySelectionOffsetCheck[
    offsetof(SegmentTopologyNode, selected_branch) == 0x18 ? 1 : -1];
typedef char SegmentTopologyContainerRootOffsetCheck[
    offsetof(SegmentTopologyContainer, root) == 0x10 ? 1 : -1];
typedef char SegmentWorkBufferSizeCheck[
    sizeof(SegmentWorkBuffer) == 0x0C ? 1 : -1];

int
set_trailing_segment_direction_samples(SegmentTopologyNode *node,
                                       int                  sample_count,
                                       int                  direction);
void
set_all_segment_direction_samples(SegmentTopologyNode *node,
                                  int                  unused_count,
                                  int                  direction);
SegmentResourceRecord *
clone_segment_resource_record(const SegmentResourceRecord *source);
SegmentTopologyNode *
create_segment_topology_leaf(SegmentResourceRecord *record);
SegmentTopologyNode *
allocate_segment_topology_junction(SegmentTopologyKind kind);
void
redirect_segment_topology_references(SegmentTopologyNode *node,
                                     SegmentTopologyNode *replacement);
SegmentTopologyNode *
find_segment_branch_reference_parent(SegmentTopologyNode  *node,
                                     SegmentBranchSelector selector);
int
connect_secondary_segment_junction(SegmentTopologyNode *primary_leaf,
                                   SegmentTopologyNode *secondary_leaf,
                                   SegmentTopologyNode *reference);
void
repair_segment_topology_reference(SegmentTopologyNode *reference);
void
set_segment_boundary_modes(SegmentTopologyNode *node,
                           int                  primary_mode,
                           int                  secondary_mode);
void
repair_secondary_segment_topology(SegmentTopologyNode *node);
int
read_segment_boundary_modes(const SegmentTopologyNode *node,
                            int                        read_front,
                            unsigned int              *primary_mode,
                            unsigned int              *secondary_mode);
void
reconcile_segment_junction_boundaries(SegmentTopologyNode *node);
void
normalize_segment_leaf_resource(SegmentTopologyNode *node,
                                int                  unused_record_address);
void
relocate_and_repair_segment_topology(SegmentTopologyNode       *node,
                                     SegmentResourceCollection *resources);
SegmentTopologyNode *
relocate_segment_topology_offsets(SegmentTopologyContainer *container,
                                  int                       node_offset);
SegmentResourceCollection *
relocate_segment_resource_collection(SegmentResourceCollection *resources);
SegmentWorkBuffer *
allocate_segment_work_buffer(int payload_bytes);
char *
choose_random_byte_address(char *first,
                           char *last);
SegmentTopologyNode *
finalize_loaded_segment_topology(void);

#endif
