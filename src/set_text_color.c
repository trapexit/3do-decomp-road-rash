/* Reconstructed from the original function at 0x00028850. */



#include "rw_semantic_data.h"

enum TextColorTableConstants
{
  TEXT_COLOR_CHANNEL_MASK = 0xFF,
  TEXT_COLOR_GRADIENT_COUNT = 4,
  TEXT_COLOR_GRADIENT_ENTRY_BYTES = 8,
  TEXT_COLOR_GRADIENT_ODD_OFFSET = 4,
  TEXT_COLOR_GRADIENT_EVEN_OFFSET = 6,
  TEXT_COLOR_FACTOR_STEP = 2,
  TEXT_COLOR_SCALE_SHIFT = 3,
  TEXT_COLOR_FIVE_BIT_MASK = 0xF8,
  TEXT_COLOR_RED_PACK_SHIFT = 7,
  TEXT_COLOR_GREEN_PACK_SHIFT = 2,
  TEXT_COLOR_BLUE_PACK_SHIFT = 3,
  TEXT_COLOR_HIGH_BIT = 0x8000,
  TEXT_COLOR_NONZERO_LOW = 1,
  TEXT_COLOR_NONZERO_HIGH = 0x8001,
  TEXT_COLOR_BYTE_SHIFT = 8
};

// Compare the resource bytes, not font RGB metadata: aliases may change a PLUT.
// Byte accesses also preserve support for unaligned resource palettes.
#define store_be16_if_changed(ptr_, value_)                                \
  do                                                                      \
    {                                                                     \
      u8 *color_bytes = (ptr_);                                            \
      u32 color_value = (value_);                                          \
                                                                          \
      if((color_bytes[0] != (color_value >> TEXT_COLOR_BYTE_SHIFT)) ||       \
         (color_bytes[1] != (color_value & TEXT_COLOR_CHANNEL_MASK)))       \
        {                                                                 \
          color_bytes[0] = (u8)(color_value >> TEXT_COLOR_BYTE_SHIFT);       \
          color_bytes[1] = (u8)color_value;                                \
        }                                                                 \
    }                                                                     \
  while(0)

void
set_text_color(int red_component_,
               int green_component_,
               int blue_component_)
{
  u32 red;
  u32 green;
  u32 blue;
  u32 index;
  u32 factor;
  u32 packed;
  TextFontEntry *font;
  u8 *entry;
  u8 *table;

  red = ((u32)red_component_ & TEXT_COLOR_CHANNEL_MASK);
  green = ((u32)green_component_ & TEXT_COLOR_CHANNEL_MASK);
  blue = ((u32)blue_component_ & TEXT_COLOR_CHANNEL_MASK);

  font = text_manager->active_font;
  if(font == NULL)
    {
      return;
    }

  table = (u8 *)font->resource_data->glyph_plut;
  font->red = (u8)red;
  font->green = (u8)green;
  font->blue = (u8)blue;

  index = 0;
  do
    {
      entry = (table + (index * TEXT_COLOR_GRADIENT_ENTRY_BYTES));
      store_be16_if_changed(entry, 0);

      factor = (1 + (index * TEXT_COLOR_FACTOR_STEP));
      packed = ((((red * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                 TEXT_COLOR_FIVE_BIT_MASK) << TEXT_COLOR_RED_PACK_SHIFT);
      packed |= ((((green * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                  TEXT_COLOR_FIVE_BIT_MASK) << TEXT_COLOR_GREEN_PACK_SHIFT);
      packed |= ((((blue * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                  TEXT_COLOR_FIVE_BIT_MASK) >> TEXT_COLOR_BLUE_PACK_SHIFT);
      if(packed == 0)
        {
          packed = TEXT_COLOR_NONZERO_LOW;
        }

      store_be16_if_changed((entry + TEXT_COLOR_GRADIENT_ODD_OFFSET), packed);

      factor = (TEXT_COLOR_FACTOR_STEP + (index * TEXT_COLOR_FACTOR_STEP));
      packed = TEXT_COLOR_HIGH_BIT;
      packed |= ((((red * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                  TEXT_COLOR_FIVE_BIT_MASK) << TEXT_COLOR_RED_PACK_SHIFT);
      packed |= ((((green * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                  TEXT_COLOR_FIVE_BIT_MASK) << TEXT_COLOR_GREEN_PACK_SHIFT);
      packed |= ((((blue * factor) >> TEXT_COLOR_SCALE_SHIFT) &
                  TEXT_COLOR_FIVE_BIT_MASK) >> TEXT_COLOR_BLUE_PACK_SHIFT);
      if(packed == TEXT_COLOR_HIGH_BIT)
        {
          packed = TEXT_COLOR_NONZERO_HIGH;
        }

      store_be16_if_changed((entry + TEXT_COLOR_GRADIENT_EVEN_OFFSET), packed);
      index++;
    }
  while(index < TEXT_COLOR_GRADIENT_COUNT);
}


#undef store_be16_if_changed
