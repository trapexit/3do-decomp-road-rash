#include "rw_semantic_data.h"

void
set_display_vbl_synchronization_enabled(int enabled)
{
  display_runtime_globals.synchronize_to_vbl = (unsigned char)enabled;
}
