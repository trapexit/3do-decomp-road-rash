#ifndef ROADRASH_ROAD_SURFACE_ASSET_PANIC_H
#define ROADRASH_ROAD_SURFACE_ASSET_PANIC_H

#include "road_geometry.h"

struct RoadRenderSide;

typedef enum RoadSurfaceAssetFailure
{
  ROAD_SURFACE_ASSET_FAMILY_UNAVAILABLE = 1,
  ROAD_SURFACE_ASSET_ENTRY_MISSING,
  ROAD_SURFACE_ASSET_DIMENSION_SELECTION_FAILED
} RoadSurfaceAssetFailure;

void
panic_missing_road_surface_asset(
  const struct RoadRenderSide *road_side,
  RoadSide                     side,
  int                          selector,
  int                          family_index,
  int                          child_index,
  int                          width,
  int                          height,
  RoadSurfaceAssetFailure      failure);

#endif
