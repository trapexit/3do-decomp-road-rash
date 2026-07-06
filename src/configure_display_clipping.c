
#include "graphics.h"
#include "rw_semantic_data.h"

enum DisplayClippingConstant
{
  DISPLAY_CLIPPING_SCREEN_WIDTH = 0x140,
  DISPLAY_CLIPPING_SCREEN_HEIGHT = 0xF0
};

int
configure_display_clipping(int width,
                           int height)
{
  int *targets;
  int x;
  int y;

  targets = (int *)(int)&(display_runtime_globals.bitmap_items[0]);

  SetClipOrigin(targets[0], 0, 0);
  SetClipOrigin(targets[1], 0, 0);
  SetClipWidth(targets[0], width);
  SetClipWidth(targets[1], width);
  SetClipHeight(targets[0], height);
  SetClipHeight(targets[1], height);

  x = (DISPLAY_CLIPPING_SCREEN_WIDTH - width) >> 1;
  y = (DISPLAY_CLIPPING_SCREEN_HEIGHT - height) >> 1;
  if((y & 1) != 0)
    {
      y &= ~1;
    }

  SetClipOrigin(targets[0], x, y);
  return SetClipOrigin(targets[1], x, y);
}
