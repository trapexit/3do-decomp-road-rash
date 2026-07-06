#include "front_end_cel_runtime.h"

void
parse_cans_animation_metadata(CansAnimationData     *animation,
                              CansAnimationMetadata *metadata)
{
  metadata->ccb = 0;
  metadata->plut = 0;
  metadata->pixel_data = 0;
  metadata->center_x = 0;
  metadata->center_y = 0;
  metadata->hotspot_count = 0;
  metadata->hotspots = 0;

  CANS_GetAnimFrame(animation,
                   CANS_METADATA_FRAME,
                   (CansAnimationFrameOutput *)metadata);
}
