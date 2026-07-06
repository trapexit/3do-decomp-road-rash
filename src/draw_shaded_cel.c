#include "display_cel_runtime.h"

void
draw_shaded_cel(const CCB *source)
{
  append_shaded_cel(source);

  if(display_runtime_globals.active_bitmap_index !=
     display_runtime_globals.display_buffer_index &&
     display_runtime_globals.enabled != 0)
    {
      return;
    }

  submit_cel_batch();
}


void
translate_display_rectangle(DisplayRectangle *rectangle,
                            int               horizontal_offset,
                            int               vertical_offset)
{
  rectangle->left += horizontal_offset;
  rectangle->right += horizontal_offset;
  rectangle->top += vertical_offset;
  rectangle->bottom += vertical_offset;
}


void
set_display_rectangle(DisplayRectangle *rectangle,
                      int               left,
                      int               top,
                      int               right,
                      int               bottom)
{
  rectangle->left = left;
  rectangle->top = top;
  rectangle->right = right;
  rectangle->bottom = bottom;
}
