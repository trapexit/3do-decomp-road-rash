#include "rw_semantic_data.h"

int
unload_text_font(int font_identifier)
{
  TextFontEntry *font;
  TextFontResourceImage *resource_image;

  if(font_identifier == 0 || text_manager == 0)
    {
      return 0;
    }

  font = text_manager->font_head;
  if(font == 0)
    {
      return 0;
    }

  do
    {
      if(font->font_identifier == font_identifier)
        {
          detach_text_font_entry(font);
          if(text_manager->active_font == font)
            {
              select_first_text_font();
            }
          if(font->owned_resource_data != 0)
            {
              resource_image = (TextFontResourceImage *)(
                (char *)font->owned_resource_data -
                offsetof(TextFontResourceImage, font));
              dispose_text_memory(resource_image, 0, 0);
            }
          dispose_text_memory(font, 0, 0);
          return 1;
        }
      font = font->next;
    } while(font != 0);

  return 0;
}
