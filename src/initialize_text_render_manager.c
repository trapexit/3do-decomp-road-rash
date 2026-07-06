#include "mem.h"

#include "rw_semantic_data.h"

int
initialize_text_render_manager(void)
{
  TextRenderManager *manager;

  if(text_manager != 0)
    {
      return 1;
    }

  manager = (TextRenderManager *)allocate_text_memory(
    sizeof(TextRenderManager), MEMTYPE_FILL);
  if(manager == 0)
    {
      return 0;
    }
  text_manager = manager;

  manager->font_head = 0;
  manager->font_tail = 0;
  manager->active_font = 0;
  manager->draw_flags = 0;
  manager->render_flags = 0;
  manager->last_font_identifier = 0;
  return 1;
}

void
dispose_text_render_manager(void)
{
  if(text_manager == 0)
    {
      return;
    }
  while(text_manager->font_head != 0)
    {
      unload_text_font(text_manager->font_head->font_identifier);
    }
  dispose_text_memory(text_manager, 0, 0);
  text_manager = 0;
}
