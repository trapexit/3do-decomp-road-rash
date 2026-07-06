#include "mem.h"

#include "rw_semantic_data.h"

static
void
append_text_font(TextRenderManager *manager,
                 TextFontEntry     *font)
{
  TextFontEntry *previous_tail;

  previous_tail = manager->font_tail;
  manager->active_font = font;
  manager->font_tail = font;
  if(manager->font_head == 0)
    {
      manager->font_head = font;
    }
  manager->last_font_identifier++;
  if(previous_tail != 0)
    {
      previous_tail->next = font;
      font->previous = previous_tail;
    }
  font->font_identifier = manager->last_font_identifier;
}


static
void
initialize_text_font_entry(TextFontEntry *font)
{
  font->next = 0;
  font->previous = 0;
  font->owned_resource_data = 0;
  font->red = TEXT_COLOR_COMPONENT_MAX;
  font->green = TEXT_COLOR_COMPONENT_MAX;
  font->blue = TEXT_COLOR_COMPONENT_MAX;
  font->shadow_x_offset = 0;
  font->shadow_y_offset = 0;
}


int
load_text_font(const char *path)
{
  void *resource_image;
  TextFontResourceImage *resource_image_header;
  TextFontResourceData *resource;
  TextFontEntry *font;
  int resource_size;

  if(path == 0 || text_manager == 0)
    {
      return 0;
    }
  resource_image = load_text_resource_file(path, MEMTYPE_CEL, 0,
                                           &resource_size);
  if(resource_image == 0)
    {
      return 0;
    }
  if(resource_size < sizeof(TextFontResourceImage))
    {
      dispose_text_memory(resource_image, 0, 0);
      return 0;
    }
  font = (TextFontEntry *)allocate_text_memory(sizeof(TextFontEntry),
                                               MEMTYPE_FILL);
  if(font == 0)
    {
      dispose_text_memory(resource_image, 0, 0);
      return 0;
    }
  initialize_text_font_entry(font);

  resource_image_header = (TextFontResourceImage *)resource_image;
  resource = &resource_image_header->font;
  append_text_font(text_manager, font);
  font->owned_resource_data = resource;
  font->resource_data = resource;
  font->fallback_glyph_width =
    (unsigned char)(resource->line_height >> 1);
  font->glyph_pixc_override = 0;

  relocate_text_font_resource(font);
  select_text_font(font->font_identifier);
  return font->font_identifier;
}


int
remove_text_font_entry(int font_identifier)
{
  TextFontEntry *font;

  if(font_identifier == 0 || text_manager == 0)
    {
      return 0;
    }

  font = text_manager->font_head;
  while(font != 0)
    {
      if(font->font_identifier == font_identifier)
        {
          detach_text_font_entry(font);
          if(text_manager->active_font == font)
            {
              select_first_text_font();
            }
          dispose_text_memory(font, 0, 0);
          return 1;
        }
      font = font->next;
    }
  return 0;
}


int
register_text_font_resource(void *resource_image)
{
  TextFontResourceImage *resource_image_header;
  TextFontResourceData *resource;
  TextFontEntry *font;

  if(resource_image == 0 || text_manager == 0)
    {
      return 0;
    }
  font = (TextFontEntry *)allocate_text_memory(sizeof(TextFontEntry),
                                               MEMTYPE_FILL);
  if(font == 0)
    {
      return 0;
    }

  initialize_text_font_entry(font);
  append_text_font(text_manager, font);
  resource_image_header = (TextFontResourceImage *)resource_image;
  resource = &resource_image_header->font;
  font->resource_data = resource;
  font->fallback_glyph_width =
    (unsigned char)(resource->line_height >> 1);
  font->glyph_pixc_override = 0;

  relocate_text_font_resource(font);
  select_text_font(font->font_identifier);
  return font->font_identifier;
}
