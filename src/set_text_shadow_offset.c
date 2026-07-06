#include "rw_semantic_data.h"

void
set_text_shadow_offset(int horizontal,
                       int vertical)
{
  TextFontEntry *font;

  if(text_manager == 0)
    {
      return;
    }
  font = text_manager->active_font;
  if(font != 0)
    {
      font->shadow_x_offset = horizontal;
      font->shadow_y_offset = vertical;
    }
}


void
select_first_text_font(void)
{
  int index;

  if(text_manager == 0)
    {
      return;
    }
  text_manager->active_font = text_manager->font_head;
  if(text_manager->active_font != 0)
    {
      select_text_font(text_manager->active_font->font_identifier);
    }
  else
    {
      for(index = 0; index < TEXT_GLYPH_CCB_COUNT; index++)
        {
          text_manager->glyph_ccbs[index] = 0;
        }
    }
}


void
unlink_text_font_neighbors(TextFontEntry *font)
{
  if(font->next != 0)
    {
      font->next->previous = font->previous;
    }

  if(font->previous != 0)
    {
      font->previous->next = font->next;
    }
}
