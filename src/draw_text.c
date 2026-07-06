
#include "rw_semantic_data.h"

int
draw_text(int bitmap_item,
          int x,
          int y,
          int text_address,
          int text_flags,
          int alignment)
{
  TextFontEntry *font;
  int mode;
  int width;

  width = 0;
  mode = (short)alignment;
  font = text_manager->active_font;
  if(font == 0)
    {
      return 0;
    }

  if(mode == TEXT_ALIGNMENT_CENTER)
    {
      width = measure_text_width((const unsigned char *)text_address,
                                 text_flags);
      width = (width +
               (int)((unsigned int)width >> TEXT_WIDTH_SIGN_SHIFT)) >> 1;
    }
  else if(mode == TEXT_ALIGNMENT_RIGHT)
    {
      width = measure_text_width((const unsigned char *)text_address,
                                 text_flags);
    }
  x -= width;

  if(font->shadow_x_offset != 0 || font->shadow_y_offset != 0)
    {
      apply_text_shadow_glyph_plut();
      draw_text_glyphs(bitmap_item,
                       x + font->shadow_x_offset,
                       y + font->shadow_y_offset,
                       (const unsigned char *)text_address,
                       text_flags,
                       mode);
      restore_active_font_glyph_plut();
    }

  return draw_text_glyphs(bitmap_item, x, y,
                          (const unsigned char *)text_address,
                          text_flags, mode);
}
