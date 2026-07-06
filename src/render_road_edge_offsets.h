#pragma once

/* Raw layout offsets extracted verbatim from render_road_edge_transition.c.
 * This header has a single consumer; the values are not proven against any
 * structure and remain offset-oriented until aggregate evidence exists. */

#define ROAD_WORD_BYTES                       4
#define ROAD_OBJECT_CONTEXT_OFFSET            0x08
#define ROAD_CONTEXT_RESOURCE_COUNTER_OFFSET  0x08
#define ROAD_CONTEXT_HORIZON_OFFSET           0x10
#define ROAD_OBJECT_DESCRIPTION_OFFSET        0x0C
#define ROAD_OBJECT_WORLD_X_OFFSET            0x10
#define ROAD_OBJECT_WORLD_Y_OFFSET            0x14
#define ROAD_OBJECT_PROJECTION_DEPTH_OFFSET   0x18
#define ROAD_OBJECT_CHAIN_STEP_OFFSET         0x20
#define ROAD_OBJECT_TRACK_Y_OFFSET            0x4C
#define ROAD_OBJECT_OUTER_X_OFFSET            0x50
#define ROAD_OBJECT_OUTER_Y_OFFSET            0x54
#define ROAD_OBJECT_INNER_X_OFFSET            0x58
#define ROAD_OBJECT_INNER_Y_OFFSET            0x5C
#define ROAD_OBJECT_FAR_X_OFFSET              0x60
#define ROAD_OBJECT_FAR_EDGE_X_OFFSET         0x68
#define ROAD_SIDE_CHAIN_OFFSET                0x74
#define ROAD_SIDE_CHAIN_HEIGHT_OFFSET         0x7C
#define ROAD_SIDE_POSITION_OFFSET             0xA0
#define ROAD_SIDE_END_OFFSET                  0xA4
#define ROAD_OBJECT_CEL_LIST_OFFSET           0xFC
#define ROAD_DESCRIPTION_X_OFFSET             0x12C
#define ROAD_DESCRIPTION_LEFT_SPAN_OFFSET     0x184
#define ROAD_DESCRIPTION_RIGHT_SPAN_OFFSET    0x188
#define ROAD_DESCRIPTION_Y_OFFSET             0x18C
#define ROAD_DESCRIPTION_STYLE_OFFSET         0x194
#define ROAD_DESCRIPTION_CEL_OFFSET           0x196
#define ROAD_DESCRIPTION_FLAGS_OFFSET         0x37
#define ROAD_CEL_PRIMARY_LIST_OFFSET          0x1C
#define ROAD_CEL_EDGE_LIST_OFFSET             0x24
#define ROAD_CEL_JOIN_LIST_OFFSET             0x2C
#define ROAD_CEL_BRIDGE_LIST_OFFSET           0x34
#define ROAD_STATE_LEFT_OFFSET                0x14
#define ROAD_STATE_RIGHT_OFFSET               0x18
#define ROAD_STATE_KIND_OFFSET                0x2C
#define ROAD_STATE_LEFT_ANCHOR_OFFSET         0x238
#define ROAD_STATE_RIGHT_ANCHOR_OFFSET        0x23C
#define ROAD_CEL_HEIGHT_BYTE_OFFSET           2
