#include "display_cel_runtime.h"

enum BackgroundCelClipConstant
{
  BACKGROUND_CEL_DIMENSION_MASK = 0x1FF,
  BACKGROUND_CEL_DIMENSION_EXPONENT_MASK = 0x1F
};

int
clip_cel_source_rectangle(const CCB   *source,
                          int          left_skip,
                          int          top_skip,
                          unsigned int width,
                          unsigned int height,
                          CCB         *destination);

static
int
decode_background_cel_dimension(int field)
{
  if(field >= 0)
    {
      return (int)((unsigned int)field &
                   BACKGROUND_CEL_DIMENSION_MASK);
    }
  return 1 << (field & BACKGROUND_CEL_DIMENSION_EXPONENT_MASK);
}


int
draw_clipped_background_cel(const CCB              *source,
                            int                     x,
                            int                     y,
                            const DisplayRectangle *clip_bounds)
{
  CCB clipped_cel;
  const CCB *draw_source;
  CCB *packet;
  int left_skip;
  int top_skip;
  int source_width;
  int source_height;
  int right;
  int bottom;
  int drawable_width;
  int drawable_height;

  if(source == 0)
    {
      return 0;
    }

  left_skip = clip_bounds->left - x;
  if(left_skip < 0)
    {
      left_skip = 0;
    }
  top_skip = clip_bounds->top - y;
  if(top_skip < 0)
    {
      top_skip = 0;
    }

  source_width = decode_background_cel_dimension(source->ccb_Width);
  source_height = decode_background_cel_dimension(source->ccb_Height);

  right = x + source_width;
  if(right > clip_bounds->right)
    {
      right = clip_bounds->right;
    }
  bottom = y + source_height;
  if(bottom > clip_bounds->bottom)
    {
      bottom = clip_bounds->bottom;
    }

  drawable_width = right - (x + left_skip);
  drawable_height = bottom - (y + top_skip);
  if(drawable_height <= 0 || drawable_width <= 0)
    {
      return 0;
    }

  draw_source = source;
  if(left_skip != 0 || top_skip != 0 ||
     drawable_width < source_width ||
     drawable_height < source_height)
    {
      clip_cel_source_rectangle(
        source, left_skip, top_skip,
        (unsigned int)drawable_width,
        (unsigned int)drawable_height, &clipped_cel);
      draw_source = &clipped_cel;
    }

  packet = append_unmapped_cel(draw_source);
  map_cel_at_native_scale(packet, x + left_skip, y + top_skip);
  return 1;
}
