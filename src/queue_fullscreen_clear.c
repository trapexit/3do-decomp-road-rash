#include "display_cel_runtime.h"

CCB *
queue_fullscreen_clear(void)
{
  DisplayRectangle rectangle;

  rectangle.left = 0;
  rectangle.top = 0;
  rectangle.right = DISPLAY_WIDTH;
  rectangle.bottom = DISPLAY_HEIGHT;
  return append_solid_rectangle_cel(&rectangle, 0);
}
