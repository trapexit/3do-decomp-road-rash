#include "rw_semantic_data.h"

void
apply_text_shadow_glyph_plut(void)
{
  TextRenderManager *manager;

  manager = text_manager;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_PLUTPtr = text_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_PLUTPtr = text_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_PLUTPtr = text_plut;
  manager->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_PLUTPtr = text_plut;
}
