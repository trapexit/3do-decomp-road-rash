#include "hardware.h"

#include "rw_semantic_data.h"

#define TEXT_GLYPH_PIXC_OVERRIDE ((int)0xFFFFAB54U)

int
select_text_font(int font_identifier)
{
  TextFontEntry *font;

  if(font_identifier == 0 || text_manager == 0)
    {
      return 0;
    }

  font = text_manager->font_head;
  if(font == 0)
    {
      return 0;
    }

  while(font != 0)
    {
      if(font->font_identifier == font_identifier)
        {
          text_manager->active_font = font;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST] =
            font->resource_data->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST];
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND] =
            font->resource_data->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND];
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD] =
            font->resource_data->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD];
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH] =
            font->resource_data->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH];

          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_Flags |=
            CCB_ACE;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_Flags |=
            CCB_ACE;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_Flags |=
            CCB_ACE;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_Flags |=
            CCB_ACE;

          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_Flags &=
            ~CCB_LDPLUT;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_Flags &=
            ~CCB_LDPLUT;
          text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_Flags &=
            ~CCB_LDPLUT;
          return 1;
        }
      font = font->next;
    }

  return 0;
}


int
set_text_fallback_glyph_width(int glyph_width)
{
  TextFontEntry *font;
  int previous_width;

  previous_width = 0;
  font = text_manager->active_font;
  if(font != 0)
    {
      previous_width = font->fallback_glyph_width;
      font->fallback_glyph_width = (unsigned char)glyph_width;
    }
  return previous_width;
}


void
set_text_glyph_pixc_override(int enabled)
{
  TextFontEntry *font;
  int pixc;

  font = text_manager->active_font;
  if(font == 0)
    {
      return;
    }

  pixc = 0;
  if((enabled & TEXT_COLOR_COMPONENT_MAX) != 0)
    {
      pixc = TEXT_GLYPH_PIXC_OVERRIDE;
    }

  font->glyph_pixc_override = pixc;
  text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_PIXC =
    (uint32)pixc;
  text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_PIXC =
    (uint32)pixc;
  text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_PIXC =
    (uint32)pixc;
  text_manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_PIXC =
    (uint32)pixc;
}


#undef TEXT_GLYPH_PIXC_OVERRIDE
