#include "bss_early_data.h"
#include "front_end_cel_runtime.h"

void
CANS_NewGetAnimFrame(CansOffsetAnimation      *animation,
                     int                       frame,
                     CansAnimationFrameOutput *output)
{
  CansReducedPixelDataChunk *frame_chunk;
  FrontEndCelOffset *center;

  output->ccb = &animation->ccb_chunk->ccb;
  output->plut = animation->plut_chunk->entries;

  frame_chunk = animation->frame_chunks[frame];
  if(frame_chunk->tag != CANS_CHUNK_TAG_RPDT)
    {
      output->ccb = 0;
      return;
    }

  output->pixel_data = (CelData *)frame_chunk->pixels;
  if(animation->frame_centers == 0)
    {
      output->center_x = 0;
      output->center_y = 0;
    }
  else
    {
      center = animation->frame_centers[frame];
      output->center_x = center->x;
      output->center_y = center->y;
    }

  output->hotspot_count = 0;
  output->animation_flags = 0;
  output->ccb->ccb_PLUTPtr = output->plut;
  output->ccb->ccb_SourcePtr = output->pixel_data;
  output->ccb->ccb_Width =
    gCelDimensionPreambleTable[frame_chunk->width_preamble_index];
  output->ccb->ccb_Height =
    gCelDimensionPreambleTable[frame_chunk->height_preamble_index];
}
