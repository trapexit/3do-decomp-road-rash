#include "display_system_runtime.h"
#include "rw_semantic_data.h"

/* Reconstructed from the original function at 0x000295B8. */
int
get_display_screen_item(int screen_index)
{
  return display_system_globals.screen_items[(unsigned char)screen_index];
}
