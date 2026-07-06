#ifndef ROADRASH_ROAD_PROJECTED_CEL_H
#define ROADRASH_ROAD_PROJECTED_CEL_H

#include "graphics_ccb.h"

#ifndef ROADRASH_ROAD_CEL_BOUNDS_TYPEDEF
  #define ROADRASH_ROAD_CEL_BOUNDS_TYPEDEF
typedef struct RoadCelBounds RoadCelBounds;
#endif

typedef struct RoadProjectedRenderClass
{
  unsigned char reserved_00[0x3C];
  unsigned char kind;
} RoadProjectedRenderClass;

typedef struct RoadProjectedRenderSample
{
  unsigned char reserved_00[0x0C];
  RoadProjectedRenderClass *render_class;
  int projected_x;
  int projected_y;
} RoadProjectedRenderSample;

typedef struct RoadProjectedRenderClip
{
  unsigned char reserved_00[0x10];
  int top;
} RoadProjectedRenderClip;

typedef struct RoadProjectedRenderContext
{
  RoadProjectedRenderSample *interpolation_target;
  int reserved_04;
  RoadProjectedRenderClip *clip;
  RoadProjectedRenderClass *render_class;
  int projected_x;
  int projected_y;
  int reciprocal_index_bias;
  int view_angle;
} RoadProjectedRenderContext;

typedef struct RoadProjectedCelExtents
{
  int horizontal;
  int vertical;
} RoadProjectedCelExtents;

typedef struct RoadProjectedCelDescriptor
{
  CCB *source_ccb;
  void *palette;
  int horizontal_extent;
  int vertical_extent;
  unsigned int flags;
  int mapping_parameter;
} RoadProjectedCelDescriptor;

void
render_road_projected_cel(RoadProjectedRenderContext *render_context,
                          RoadProjectedCelDescriptor *descriptor,
                          int                         scale,
                          int                         depth,
                          int                         vertical_position,
                          int                         interpolation,
                          int                         mode);
void
map_rotated_road_bounds_to_cel(CCB                     *cel,
                               const RoadCelBounds     *bounds,
                               RoadProjectedCelExtents *extents,
                               int                      angle);

#endif
