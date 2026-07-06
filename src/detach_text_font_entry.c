#include "rw_semantic_data.h"

void
detach_text_font_entry(TextFontEntry *font)
{
  TextRenderManager *manager;

  manager = text_manager;
  if(font == manager->font_head)
    {
      manager->font_head = font->next;
    }
  if(font == manager->font_tail)
    {
      manager->font_tail = font->previous;
    }

  unlink_text_font_neighbors(font);
}
