#include "rw_semantic_data.h"

void
restore_active_font_glyph_plut(void)
{
  TextRenderManager *manager;
  TextFontEntry *font;
  void *glyph_plut;

  manager = text_manager;
  font = manager->active_font;
  if(font == 0)
    {
      return;
    }

  glyph_plut = font->resource_data->glyph_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_PLUTPtr = glyph_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_PLUTPtr = glyph_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_PLUTPtr = glyph_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_PLUTPtr = glyph_plut;
}
