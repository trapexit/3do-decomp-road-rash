#include "string.h"

#include "rw_semantic_data.h"

#define TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT 8

int
measure_text_width(const unsigned char *text,
                   int                  character_spacing)
{
  TextFontEntry *font;
  const unsigned char *glyph_width_bytes;
  int length;
  int index;
  int width;
  int glyph;

  width = 0;
  font = text_manager->active_font;
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
          width += font->fallback_glyph_width;
        }
      else
        {
          glyph_width_bytes =
            font->resource_data->glyph_width_bytes[glyph];
          width += (short)(
            ((unsigned int)glyph_width_bytes[0] <<
              TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT) |
            glyph_width_bytes[1]);
        }
      width += character_spacing;
      index++;
    } while(index < length);

  return width;
}


#undef TEXT_GLYPH_WIDTH_HIGH_BYTE_SHIFT
