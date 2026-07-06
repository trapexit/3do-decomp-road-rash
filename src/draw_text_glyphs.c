#include "string.h"

#include "display_cel_runtime.h"
#include "rw_semantic_data.h"

#define TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT 8

int
draw_text_glyphs(int                  bitmap_item,
                 int                  x,
                 int                  y,
                 const unsigned char *text,
                 int                  character_spacing,
                 int                  layout)
{
  TextRenderManager *manager;
  TextFontEntry *font;
  TextFontResourceData *resource;
  const unsigned char *glyph_width_bytes;
  int length;
  int index;
  int glyph;
  int glyph_width;
  int draw_layout;

  (void)bitmap_item;

  draw_layout = (short)layout;
  manager = text_manager;
  font = manager->active_font;
  if(font == 0)
    {
      return 0;
    }

  length = (int)strlen((const char *)text);
  index = 0;
  if(length <= 0)
    {
      return 0;
    }

  do
    {
      glyph = text[index] - TEXT_CHARACTER_BASE;
      if(glyph <= 0 || glyph >= TEXT_GLYPH_INDEX_LIMIT)
        {
          glyph_width = font->fallback_glyph_width;
        }
      else
        {
          resource = font->resource_data;
          glyph_width_bytes = resource->glyph_width_bytes[glyph];
          glyph_width = (short)(
            ((unsigned int)glyph_width_bytes[0] <<
              TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT) |
            glyph_width_bytes[1]);

          if(resource->glyph_sources[TEXT_GLYPH_LAYER_FIRST][glyph] != 0)
            {
              manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_SourcePtr =
                resource->glyph_sources[TEXT_GLYPH_LAYER_FIRST][glyph];
              manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_SourcePtr =
                resource->glyph_sources[TEXT_GLYPH_LAYER_SECOND][glyph];
              manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_SourcePtr =
                resource->glyph_sources[TEXT_GLYPH_LAYER_THIRD][glyph];
              manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_SourcePtr =
                resource->glyph_sources[TEXT_GLYPH_LAYER_FOURTH][glyph];

              map_cel_at_native_scale(
                append_unmapped_cel(
                  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]), x, y);
              map_cel_at_native_scale(
                append_unmapped_cel(
                  manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]), x, y);
              map_cel_at_native_scale(
                append_unmapped_cel(
                  manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]), x, y);
              map_cel_at_native_scale(
                append_unmapped_cel(
                  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]), x, y);
            }
        }

      if(draw_layout == TEXT_LAYOUT_VERTICAL)
        {
          y += font->resource_data->line_height + character_spacing;
        }
      else
        {
          x += glyph_width + character_spacing;
        }
      index++;
    } while(index < length);

  return 0;
}


#undef TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT
