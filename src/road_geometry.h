#ifndef ROADRASH_ROAD_GEOMETRY_H
#define ROADRASH_ROAD_GEOMETRY_H

#ifndef offsetof
  #include "stddef.h"
#endif

#include "graphics.h"

/* Road projection coordinates use signed 16.16 fixed point. */
typedef int RoadFixed;
typedef int RoadSide;
typedef int RoadOctantDirection;

struct RoadRenderSide;

enum RoadSideValue
{
  ROAD_SIDE_LEFT = 0,
  ROAD_SIDE_RIGHT = 1,
  ROAD_SIDE_COUNT = 2
};

enum RoadGeometryModeValue
{
  ROAD_GEOMETRY_MODE_TEXTURED = 0,
  ROAD_GEOMETRY_MODE_EDGE = 1,
  ROAD_GEOMETRY_MODE_RESERVED = 2,
  ROAD_GEOMETRY_MODE_CURVED = 3,
  ROAD_GEOMETRY_MODE_PROFILE = 4,
  ROAD_GEOMETRY_MODE_MAX = ROAD_GEOMETRY_MODE_PROFILE,
  ROAD_GEOMETRY_MODE_UNSET = 0xFF
};

enum RoadSurfaceFlagValue
{
  ROAD_SURFACE_FLAG_REVERSED_MARGIN = 0x08,
  ROAD_SURFACE_FLAG_DEPTH_ADJUSTED = 0x10,
  ROAD_SURFACE_FLAG_BLEND_TRANSITION = 0x20,
  ROAD_SURFACE_FLAG_MARGIN_MASK =
    ROAD_SURFACE_FLAG_REVERSED_MARGIN | ROAD_SURFACE_FLAG_DEPTH_ADJUSTED
};

enum RoadSurfaceResourceValue
{
  ROAD_SURFACE_RESOURCE_MISSING = 0xFF,
  ROAD_SURFACE_RESOURCE_HEIGHT_SHIFT = 4,
  ROAD_SURFACE_RESOURCE_MIN_HEIGHT = 0x11,
  ROAD_SURFACE_RESOURCE_SCALE_PROBE = 0x08000000,
  ROAD_SURFACE_FAMILY_GROUP = 1,
  ROAD_RAISED_SURFACE_RESOURCE_VARIANT_COUNT = 2,
  ROAD_PROCEDURAL_FAMILY_GROUP = 3,
  ROAD_PROCEDURAL_RESOURCE_MAGIC = 0x434C4750
};

enum RoadConnectionGeometryIndex
{
  ROAD_CONNECTION_CENTER_Y = 0,
  ROAD_CONNECTION_SECONDARY_Y = 1,
  ROAD_CONNECTION_LEFT_X = 2,
  ROAD_CONNECTION_RIGHT_X = 3,
  ROAD_CONNECTION_SOURCE_CENTER_Y = 4,
  ROAD_CONNECTION_SOURCE_SECONDARY_Y = 5,
  ROAD_CONNECTION_SOURCE_LEFT_X = 6,
  ROAD_CONNECTION_SOURCE_RIGHT_X = 7
};

enum RoadLaneEdgeIndex
{
  ROAD_LANE_EDGE_LEFT = 0,
  ROAD_LANE_EDGE_RIGHT = 1,
  ROAD_SHOULDER_EDGE_LEFT = 2,
  ROAD_SHOULDER_EDGE_RIGHT = 3
};

enum RoadModeGeometryIndex
{
  ROAD_MODE_GEOMETRY_LEFT_NEAR = 0,
  ROAD_MODE_GEOMETRY_RIGHT_NEAR = 1,
  ROAD_MODE_GEOMETRY_LEFT_FAR = 2,
  ROAD_MODE_GEOMETRY_RIGHT_FAR = 3
};

enum RoadTransitionGeometryIndex
{
  ROAD_TRANSITION_LEFT_X = 0,
  ROAD_TRANSITION_LEFT_Y = 1,
  ROAD_TRANSITION_RIGHT_X = 2,
  ROAD_TRANSITION_RIGHT_Y = 3
};

enum RoadOctantDirectionValue
{
  ROAD_OCTANT_UP_RIGHT_STEEP = 0,
  ROAD_OCTANT_RIGHT_UP_SHALLOW = 1,
  ROAD_OCTANT_RIGHT_DOWN_SHALLOW = 2,
  ROAD_OCTANT_DOWN_RIGHT_STEEP = 3,
  ROAD_OCTANT_DOWN_LEFT_STEEP = 4,
  ROAD_OCTANT_LEFT_DOWN_SHALLOW = 5,
  ROAD_OCTANT_LEFT_UP_SHALLOW = 6,
  ROAD_OCTANT_UP_LEFT_STEEP = 7
};

typedef struct RoadPoint
{
  RoadFixed x;
  RoadFixed y;
} RoadPoint;

/* MapCel winding order: upper-left, upper-right, lower-right, lower-left. */
typedef struct RoadQuad
{
  RoadPoint top_left;
  RoadPoint top_right;
  RoadPoint bottom_right;
  RoadPoint bottom_left;
} RoadQuad;

struct RoadCelBounds
{
  RoadFixed top;
  RoadFixed left;
  RoadFixed bottom;
  RoadFixed right;
};

#ifndef ROADRASH_ROAD_CEL_BOUNDS_TYPEDEF
  #define ROADRASH_ROAD_CEL_BOUNDS_TYPEDEF
typedef struct RoadCelBounds RoadCelBounds;
#endif

typedef struct RoadPolyline6
{
  RoadPoint points[6];
} RoadPolyline6;

/*
 * Projection work shared by the two four-way road-strip subdividers.
 * Builder code passes this object through the original int-pointer ABI.
 */
typedef struct RoadStripDrawContext
{
  RoadSide side;
  int projection_depth;
  RoadFixed horizon_y;
  int reserved;
  RoadQuad world_quad;
  CCB *cel;
  RoadQuad *screen_quad;
  unsigned char adjust_edges;
  unsigned char alignment[3];
} RoadStripDrawContext;

typedef struct RoadStripSubdivisionContext
{
  RoadStripDrawContext draw;
  RoadPoint source_profile[8];
} RoadStripSubdivisionContext;

int *
map_fixed_road_bounds_to_cel(CCB                 *cel,
                             const RoadCelBounds *bounds);
int *
map_road_bounds_to_cel(CCB                 *cel,
                       const RoadCelBounds *bounds);
int *
map_fixed_road_quad_to_cel(CCB            *cel,
                           const RoadQuad *quad);
int
project_road_world_point(RoadFixed  world_x,
                         RoadFixed  world_y,
                         int        projection_depth,
                         RoadPoint *screen_point);
int
road_quad_is_horizontally_visible(const struct RoadRenderSide *road_side,
                                  const RoadQuad              *quad,
                                  int                          reverse_direction);
int
render_road_surface_blend_strip(struct RoadRenderSide *road_side,
                                RoadSide               side,
                                int                    surface_flags);

enum RoadGeometryConstants
{
  ROAD_FIXED_FRACTION_BITS = 16,
  ROAD_8_8_FRACTION_BITS = 8,
  ROAD_8_8_FRACTION_MASK = 0xFF,
  ROAD_PROJECTION_DEPTH_STEP = 0x100,
  ROAD_LANE_SAMPLE_SPACING = 0xFA,
  ROAD_RECIPROCAL_NUMERATOR = 0x1000000,
  ROAD_PACKED_HIGH_BYTE_SHIFT = 24,
  ROAD_INTEGER_BIT_COUNT = 32,
  ROAD_TRACK_INDEX_MASK = 0x7F,
  ROAD_RENDER_TRANSITION_VISIBILITY_MASK = 0x50,
  ROAD_SUBDIVISION_QUARTER_SHIFT = 2,
  ROAD_SUBDIVISION_QUARTER_ROUNDING_BIAS =
    (1 << ROAD_SUBDIVISION_QUARTER_SHIFT) - 1,
  ROAD_SUBDIVISION_INTERMEDIATE_COUNT = 3,
  ROAD_PROFILE_BASE_POINT_COUNT = 4,
  ROAD_PROFILE_BASE_STRIP_COUNT = ROAD_PROFILE_BASE_POINT_COUNT - 1,
  ROAD_PROFILE_MAX_POINT_COUNT = ROAD_PROFILE_BASE_POINT_COUNT + 1,
  ROAD_QUAD_CORNER_COUNT = 4,
  ROAD_OCTANT_DIRECTION_COUNT = 8,
  ROAD_OPPOSITE_OCTANT_OFFSET = ROAD_OCTANT_DIRECTION_COUNT / 2,
  ROAD_QUAD_JOIN_COUNT = 16,
  ROAD_FIXED_CROSS_PRODUCT_SHIFT = 12,
  ROAD_SUBDIVISION_DEPTH_STEP = 0x40,
  ROAD_SUBDIVISION_MIN_DEPTH = 0xD3,
  ROAD_SUBDIVISION_DEPTH_LIMIT = 0x2D2,
  ROAD_TRANSITION_INSET_DEPTH_LIMIT = 0x3D2,
  ROAD_SUBDIVISION_CEL_HEIGHT_ADJUST = 2,
  ROAD_CEL_SCALE_MASK = 0x1FF,
  ROAD_SHIFT_COUNT_MASK = 0x1F
};

#define ROAD_FIXED_ONE \
        ((RoadFixed)(1 << ROAD_FIXED_FRACTION_BITS))
#define ROAD_FIXED_FROM_INTEGER(value) \
        ((RoadFixed)((value) << ROAD_FIXED_FRACTION_BITS))
#define ROAD_FIXED_HALF \
        ((RoadFixed)(ROAD_FIXED_ONE >> 1))
#define ROAD_FIXED_THREE_QUARTERS \
        ((RoadFixed)(ROAD_FIXED_ONE - (ROAD_FIXED_ONE >> 2)))
#define ROAD_FIXED_THREE_EIGHTHS \
        ((RoadFixed)(ROAD_FIXED_THREE_QUARTERS >> 1))
#define ROAD_FIXED_QUARTER \
        ((RoadFixed)(ROAD_FIXED_ONE >> 2))
#define ROAD_FIXED_FLOOR_QUARTER(value) \
        ((RoadFixed)((value) & ~(ROAD_FIXED_QUARTER - 1)))
#define ROAD_FIXED_TO_INTEGER(value) \
        ((int)((value) >> ROAD_FIXED_FRACTION_BITS))
#define ROAD_FIXED_FROM_8_8(value) \
        ((RoadFixed)((value) << 8))
#define ROAD_QUAD_POINTS(quad) \
        ((RoadPoint *)(quad))

#define ROAD_PROJECTION_SCREEN_X_CENTER \
        ROAD_FIXED_FROM_INTEGER(150)
#define ROAD_PROJECTION_SCREEN_Y_ORIGIN \
        ROAD_FIXED_FROM_INTEGER(90)
#define ROAD_SCREEN_RIGHT \
        ROAD_FIXED_FROM_INTEGER(300)
#define ROAD_SCREEN_ROAD_BOTTOM \
        ROAD_FIXED_FROM_INTEGER(230)
#define ROAD_TRANSITION_EDGE_INSET \
        ROAD_FIXED_FROM_INTEGER(4)
#define ROAD_PROJECTION_PRODUCT_LIMIT \
        ((RoadFixed)0x40000000)
#define ROAD_PROJECTION_PRODUCT_MINIMUM \
        ((RoadFixed) - ROAD_PROJECTION_PRODUCT_LIMIT)

#define ROAD_PRE0_SHADE_BITS(scale_step) \
        ((((scale_step) - 1) << PRE0_VCNT_SHIFT) & PRE0_VCNT_MASK)

#define ROAD_LAYOUT_ASSERT(name, expression) \
        typedef char name[(expression) ? 1 : -1]

ROAD_LAYOUT_ASSERT(RoadFixedSizeIs4, sizeof(RoadFixed) == 0x04);
ROAD_LAYOUT_ASSERT(RoadSideSizeIs4, sizeof(RoadSide) == 0x04);
ROAD_LAYOUT_ASSERT(RoadPointSizeIs8, sizeof(RoadPoint) == 0x08);
ROAD_LAYOUT_ASSERT(RoadQuadSizeIs32, sizeof(RoadQuad) == 0x20);
ROAD_LAYOUT_ASSERT(RoadCelBoundsSizeIs16, sizeof(RoadCelBounds) == 0x10);
ROAD_LAYOUT_ASSERT(RoadQuadTopRightOffsetIs8,
                   offsetof(RoadQuad, top_right) == 0x08);
ROAD_LAYOUT_ASSERT(RoadQuadBottomRightOffsetIs16,
                   offsetof(RoadQuad, bottom_right) == 0x10);
ROAD_LAYOUT_ASSERT(RoadQuadBottomLeftOffsetIs24,
                   offsetof(RoadQuad, bottom_left) == 0x18);
ROAD_LAYOUT_ASSERT(RoadPolyline6SizeIs48, sizeof(RoadPolyline6) == 0x30);

ROAD_LAYOUT_ASSERT(RoadStripDrawSideOffsetIs0,
                   offsetof(RoadStripDrawContext, side) == 0x00);
ROAD_LAYOUT_ASSERT(RoadStripDrawDepthOffsetIs4,
                   offsetof(RoadStripDrawContext, projection_depth) == 0x04);
ROAD_LAYOUT_ASSERT(RoadStripDrawHorizonOffsetIs8,
                   offsetof(RoadStripDrawContext, horizon_y) == 0x08);
ROAD_LAYOUT_ASSERT(RoadStripDrawWorldQuadOffsetIs16,
                   offsetof(RoadStripDrawContext, world_quad) == 0x10);
ROAD_LAYOUT_ASSERT(RoadStripDrawCelOffsetIs48,
                   offsetof(RoadStripDrawContext, cel) == 0x30);
ROAD_LAYOUT_ASSERT(RoadStripDrawScreenQuadOffsetIs52,
                   offsetof(RoadStripDrawContext, screen_quad) == 0x34);
ROAD_LAYOUT_ASSERT(RoadStripDrawAdjustOffsetIs56,
                   offsetof(RoadStripDrawContext, adjust_edges) == 0x38);
ROAD_LAYOUT_ASSERT(RoadStripDrawContextSizeIs60,
                   sizeof(RoadStripDrawContext) == 0x3C);
ROAD_LAYOUT_ASSERT(RoadStripSubdivisionSourceOffsetIs60,
                   offsetof(RoadStripSubdivisionContext, source_profile) ==
                   0x3C);
ROAD_LAYOUT_ASSERT(RoadStripSubdivisionContextSizeIs124,
                   sizeof(RoadStripSubdivisionContext) == 0x7C);

ROAD_LAYOUT_ASSERT(CcbSourceOffsetIs8, offsetof(CCB, ccb_SourcePtr) == 0x08);
ROAD_LAYOUT_ASSERT(CcbPre0OffsetIs52, offsetof(CCB, ccb_PRE0) == 0x34);
ROAD_LAYOUT_ASSERT(CcbHeightOffsetIs64, offsetof(CCB, ccb_Height) == 0x40);
ROAD_LAYOUT_ASSERT(CcbSizeIs68, sizeof(CCB) == 0x44);

#undef ROAD_LAYOUT_ASSERT

#endif
