#pragma once

/* Raw layout offsets extracted verbatim from render_recessed_road_edge.c.
 * This header has a single consumer; the values are not proven against any
 * structure and remain offset-oriented until aggregate evidence exists. */

#define ROAD_ROOT_DESCRIPTOR_OFFSET 0x0C
#define ROAD_DESCRIPTOR_TYPE_OFFSET 0x3C
#define ROAD_MASK_VALUE_OFFSET 8
#define ROAD_DESCRIPTOR_CURVE_MODE_OFFSET 0x1AC
#define ROAD_BORDER_CACHE_OFFSET 0x2C
#define ROAD_VERTICAL_EDGE_CACHE_OFFSET 0x34
#define CEL_PIXC_OFFSET 0x30
