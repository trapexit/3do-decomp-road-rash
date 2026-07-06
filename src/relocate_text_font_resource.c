#include "text_render_runtime.h"

void
relocate_text_font_resource(TextFontEntry *font)
{
  TextFontResourceData *resource;
  int glyph;

  resource = font->resource_data;
  resource->glyph_plut =
    (char *)resource + (int)resource->glyph_plut;

  resource->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST] =
    (CCB *)((char *)resource +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]);
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST]->ccb_PLUTPtr =
    resource->glyph_plut;
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND] =
    (CCB *)((char *)resource +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]);
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND]->ccb_PLUTPtr =
    resource->glyph_plut;
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD] =
    (CCB *)((char *)resource +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]);
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD]->ccb_PLUTPtr =
    resource->glyph_plut;
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH] =
    (CCB *)((char *)resource +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]);
  resource->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH]->ccb_PLUTPtr =
    resource->glyph_plut;

  glyph = 0;
  do
    {
      if(resource->glyph_sources[TEXT_GLYPH_LAYER_FIRST][glyph] != 0)
        {
          resource->glyph_sources[TEXT_GLYPH_LAYER_FIRST][glyph] =
            (char *)resource->glyph_sources[TEXT_GLYPH_LAYER_FIRST]
            [glyph] +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_FIRST] +
            offsetof(CCB, ccb_PLUTPtr);
          resource->glyph_sources[TEXT_GLYPH_LAYER_SECOND][glyph] =
            (char *)resource->glyph_sources[TEXT_GLYPH_LAYER_SECOND]
            [glyph] +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_SECOND] +
            offsetof(CCB, ccb_PLUTPtr);
          resource->glyph_sources[TEXT_GLYPH_LAYER_THIRD][glyph] =
            (char *)resource->glyph_sources[TEXT_GLYPH_LAYER_THIRD]
            [glyph] +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_THIRD] +
            offsetof(CCB, ccb_PLUTPtr);
          resource->glyph_sources[TEXT_GLYPH_LAYER_FOURTH][glyph] =
            (char *)resource->glyph_sources[TEXT_GLYPH_LAYER_FOURTH]
            [glyph] +
            (int)resource->glyph_ccbs[TEXT_GLYPH_LAYER_FOURTH] +
            offsetof(CCB, ccb_PLUTPtr);
        }
      glyph++;
    } while(glyph < TEXT_GLYPH_COUNT);
}
