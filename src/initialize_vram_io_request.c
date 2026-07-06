#include "graphics.h"

#include "display_system_runtime.h"
#include "rw_semantic_data.h"

int
initialize_vram_io_request(void)
{
  if(display_system_globals.vram_io_request <= 0)
    {
      display_system_globals.vram_io_request = GetVRAMIOReq();
    }
  return display_system_globals.vram_io_request < 0 ?
    display_system_globals.vram_io_request : 0;
}
